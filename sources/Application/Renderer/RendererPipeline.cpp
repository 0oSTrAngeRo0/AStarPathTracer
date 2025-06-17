#include "Core/DeviceContext.h"
#include "Core/RayTracingShaders.h"
#include "Application/Renderer/RendererPipeline.h"
#include "Application/Renderer/RenderContext.h"
#include "Application/Renderer/CommandUtilities.h"
#include "Engine/Components/Camera.h"
#include "Engine/Components/Transform.h"
#include "Engine/Components/Render.h"
#include "Engine/Resources/ResourcesManager.h"
#include "Engine/Resources/ResourceData.h"
#include "Engine/HostShaderManager.h"
#include "Utilities/EnumX.h"

RendererPipeline::RendererPipeline(const DeviceContext& context, const RenderContext& render) {
	descriptor_pool = CreateDescriptorPool(context);
	descriptor_set_layout = CreateDescriptorSetLayout(context.GetDevice());
	std::vector<vk::DescriptorSet> sets = AllocateDescriptorSet(context.GetDevice(), descriptor_pool, descriptor_set_layout);
	descriptor_set = sets[0];
	UpdateDescriptorSet(context, render, descriptor_set);
	CreateRayTracingPipelineLayout(context);
	CreatePipelineAndBindingTable(context);
}

std::vector<vk::DescriptorSet> RendererPipeline::AllocateDescriptorSet(
	vk::Device device, 
	vk::DescriptorPool pool, 
	vk::DescriptorSetLayout layout
) {
	std::vector<uint32_t> max_counts = {
		MaxTextureCount
	};

	vk::StructureChain<vk::DescriptorSetAllocateInfo, vk::DescriptorSetVariableDescriptorCountAllocateInfo> ci(
		vk::DescriptorSetAllocateInfo(pool, layout),
		vk::DescriptorSetVariableDescriptorCountAllocateInfo(max_counts)
	);
	return device.allocateDescriptorSets(ci.get<vk::DescriptorSetAllocateInfo>());
}

void RendererPipeline::UpdateDescriptorSet(const DeviceContext& context, const RenderContext& render, const vk::DescriptorSet& set) {
	std::vector<vk::AccelerationStructureKHR> tlases = { render.GetTlas() };
	vk::WriteDescriptorSetAccelerationStructureKHR write_tlas(tlases);
	vk::DescriptorImageInfo write_rt_image({}, render.GetOutputImageView(), vk::ImageLayout::eGeneral);
	vk::DescriptorBufferInfo write_vertex_position_buffer(render.GetVertexPositionBuffer(), 0, vk::WholeSize);
	vk::DescriptorBufferInfo write_vertex_other_buffer(render.GetVertexOtherBuffer(), 0, vk::WholeSize);
	vk::DescriptorBufferInfo write_index_buffer(render.GetIndexBuffer(), 0, vk::WholeSize);
	vk::DescriptorBufferInfo write_material_buffer(render.GetMaterialBuffer(), 0, vk::WholeSize);
	vk::DescriptorBufferInfo write_instance_buffer(render.GetInstancesBuffer(), 0, vk::WholeSize);
	vk::DescriptorBufferInfo write_constants_buffer(render.GetConstantsBuffer(), 0, vk::WholeSize);
	vk::DescriptorImageInfo write_accumulate_image({}, render.GetAccumulateImageView(), vk::ImageLayout::eGeneral);
	std::vector<vk::WriteDescriptorSet> writes = {
		vk::WriteDescriptorSet(set, 0, 0, 1, vk::DescriptorType::eAccelerationStructureKHR, {}, {}, {}, &write_tlas),
		vk::WriteDescriptorSet(set, 1, 0, vk::DescriptorType::eStorageImage, write_rt_image),
		vk::WriteDescriptorSet(set, 2, 0, vk::DescriptorType::eStorageBuffer, {}, write_vertex_position_buffer),
		vk::WriteDescriptorSet(set, 3, 0, vk::DescriptorType::eStorageBuffer, {}, write_vertex_other_buffer),
		vk::WriteDescriptorSet(set, 4, 0, vk::DescriptorType::eStorageBuffer, {}, write_index_buffer),
		vk::WriteDescriptorSet(set, 5, 0, vk::DescriptorType::eStorageBuffer, {}, write_material_buffer),
		vk::WriteDescriptorSet(set, 6, 0, vk::DescriptorType::eStorageBuffer, {}, write_instance_buffer),
		vk::WriteDescriptorSet(set, 7, 0, vk::DescriptorType::eUniformBuffer, {}, write_constants_buffer),
		vk::WriteDescriptorSet(set, 8, 0, vk::DescriptorType::eStorageImage, write_accumulate_image),
	};
	std::vector<vk::DescriptorImageInfo> write_textures = render.GetTextures();
	if (write_textures.size() > 0) {
		size_t size = write_textures.size();
		for (size_t index = 0; index < size; index++) {
			writes.emplace_back(vk::WriteDescriptorSet(set, 9, index, vk::DescriptorType::eCombinedImageSampler, write_textures[index]));
		}
	}

	context.GetDevice().updateDescriptorSets(writes, {});
}

void RendererPipeline::CmdDraw(const vk::CommandBuffer cmd, const RenderContext& render) {
	vk::Extent2D extent = render.GetOutputImageExtent();

	vk::ImageSubresourceRange subresource(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);

	CommandUtilities::CmdInsertImageBarrier(cmd, vk::ImageMemoryBarrier(
		vk::AccessFlagBits::eNone, vk::AccessFlagBits::eShaderWrite,
		vk::ImageLayout::eUndefined, vk::ImageLayout::eGeneral,
		{}, {}, render.GetOutputImage(), subresource));
	cmd.bindPipeline(vk::PipelineBindPoint::eRayTracingKHR, pipeline);
	cmd.bindDescriptorSets(vk::PipelineBindPoint::eRayTracingKHR, pipeline_layout, 0, vk::ArrayProxy(descriptor_set), {});
	cmd.traceRaysKHR(
		shader_binding_table->GetRayGen(),
		shader_binding_table->GetMiss(),
		shader_binding_table->GetClosestHit(),
		shader_binding_table->GetCallable(),
		extent.width,
		extent.height,
		1
	);
}

void RendererPipeline::CmdCopyOutputTo(const vk::CommandBuffer cmd, const RenderContext& render, const vk::Image target) {
	vk::Extent2D extent = render.GetOutputImageExtent();

	vk::ImageSubresourceRange subresource(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
	vk::ImageCopy copy_region(
		vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, 0, 0, 1),
		vk::Offset3D(0, 0, 0),
		vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, 0, 0, 1),
		vk::Offset3D(0, 0, 0),
		vk::Extent3D(extent, 1)
	);

	CommandUtilities::CmdInsertImageBarrier(cmd, vk::ImageMemoryBarrier(
		vk::AccessFlagBits::eNone, vk::AccessFlagBits::eShaderWrite,
		vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal,
		{}, {}, target, subresource));
	CommandUtilities::CmdInsertImageBarrier(cmd, vk::ImageMemoryBarrier(
		vk::AccessFlagBits::eShaderWrite, vk::AccessFlagBits::eShaderRead,
		vk::ImageLayout::eGeneral, vk::ImageLayout::eTransferSrcOptimal,
		{}, {}, render.GetOutputImage(), subresource));
	cmd.copyImage(render.GetOutputImage(), vk::ImageLayout::eTransferSrcOptimal, target, vk::ImageLayout::eTransferDstOptimal, copy_region);
	CommandUtilities::CmdInsertImageBarrier(cmd, vk::ImageMemoryBarrier(
		vk::AccessFlagBits::eNone, vk::AccessFlagBits::eShaderWrite,
		vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::ePresentSrcKHR,
		{}, {}, target, subresource));
}

vk::DescriptorSetLayout RendererPipeline::CreateDescriptorSetLayout(const vk::Device device) {
	std::vector<vk::DescriptorSetLayoutBinding> bindings = {
		vk::DescriptorSetLayoutBinding(0, vk::DescriptorType::eAccelerationStructureKHR, 1, vk::ShaderStageFlagBits::eRaygenKHR), // tlas
		vk::DescriptorSetLayoutBinding(1, vk::DescriptorType::eStorageImage, 1, vk::ShaderStageFlagBits::eRaygenKHR), // output image
		vk::DescriptorSetLayoutBinding(2, vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eClosestHitKHR), // vertex position buffer
		vk::DescriptorSetLayoutBinding(3, vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eClosestHitKHR), // vertex other buffer
		vk::DescriptorSetLayoutBinding(4, vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eClosestHitKHR), // index buffer
		vk::DescriptorSetLayoutBinding(5, vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eClosestHitKHR), // material buffer
		vk::DescriptorSetLayoutBinding(6, vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eClosestHitKHR), // instance buffer
		vk::DescriptorSetLayoutBinding(7, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eRaygenKHR), // constants buffer
		vk::DescriptorSetLayoutBinding(8, vk::DescriptorType::eStorageImage, 1, vk::ShaderStageFlagBits::eRaygenKHR), // accumulate image
		vk::DescriptorSetLayoutBinding(9, vk::DescriptorType::eCombinedImageSampler, MaxTextureCount, vk::ShaderStageFlagBits::eAll), // textures
	};

	std::vector<vk::DescriptorBindingFlags> binding_flags = {
		{}, {}, {}, {}, {}, {}, {}, {}, {},
		vk::DescriptorBindingFlagBits::ePartiallyBound | 
		vk::DescriptorBindingFlagBits::eUpdateAfterBind | 
		vk::DescriptorBindingFlagBits::eVariableDescriptorCount
	};

	vk::StructureChain<vk::DescriptorSetLayoutCreateInfo, vk::DescriptorSetLayoutBindingFlagsCreateInfo> ci(
		vk::DescriptorSetLayoutCreateInfo(vk::DescriptorSetLayoutCreateFlagBits::eUpdateAfterBindPool, bindings),
		vk::DescriptorSetLayoutBindingFlagsCreateInfo(binding_flags)
	);
	return device.createDescriptorSetLayout(ci.get<vk::DescriptorSetLayoutCreateInfo>());
}

void RendererPipeline::CreateRayTracingPipelineLayout(const DeviceContext& context) {
	vk::PipelineLayoutCreateInfo create_info({}, descriptor_set_layout);
	pipeline_layout = context.GetDevice().createPipelineLayout(create_info);
}

void RendererPipeline::CreatePipelineAndBindingTable(const DeviceContext& context) {
	std::vector<std::tuple<Uuid, RayTracingShaders::ShaderData>> shaders;
	const auto& shader_resources = HostShaderManager::GetInstance().GetAllShaders();
	ResourcesManager::GetInstance().IterateResources([&shaders](const std::string& path, const ResourceBase& resource) {
		if (resource.GetResourceTypeId() != Resource<ShaderResourceData>::type_id) return;
		const ShaderResourceData& shader = static_cast<const Resource<ShaderResourceData>&>(resource).resource_data;
		auto binary_code = ResourcesManager::GetInstance().LoadBinaryFile(shader.compiled_code_path);
		RayTracingShaders::ShaderData data(std::move(binary_code), shader.entry_function, shader.shader_stage);
		shaders.emplace_back(std::make_tuple(resource.uuid, data));
	});
	std::sort(shaders.begin(), shaders.end(), [](const auto& a, const auto& b) {
		return RayTracingShaders::StageComparer(std::get<1>(a), std::get<1>(b));
	});

	uint32_t shader_index = 0;
	for (size_t i = 0, end = shaders.size(); i < end; i++) {
		if (std::get<1>(shaders[i]).stage != vk::ShaderStageFlagBits::eClosestHitKHR) continue;
		shader_indices.insert(std::make_pair(std::get<0>(shaders[i]), shader_index));
		shader_index++;
	}

	std::vector<RayTracingShaders::ShaderData> shader_data(shaders.size());
	std::transform(shaders.begin(), shaders.end(), shader_data.begin(), [](const auto& shader) { return std::get<1>(shader); });

	RayTracingShaders::PipelineData pipeline_data(context, shader_data);
	vk::RayTracingPipelineCreateInfoKHR create_info({}, pipeline_data.GetStages(), pipeline_data.GetGroups(), 1, {}, {}, {}, pipeline_layout);
	pipeline = context.GetDevice().createRayTracingPipelineKHR({}, {}, create_info).value;

	shader_binding_table = std::make_unique<RayTracingShaders::BindingTable>(context, pipeline, pipeline_data.GetShaderCount());

	pipeline_data.Destroy(context);
}

vk::DescriptorPool RendererPipeline::CreateDescriptorPool(const DeviceContext& context) {
	std::vector<vk::DescriptorPoolSize> pool_sizes = {
		vk::DescriptorPoolSize(vk::DescriptorType::eAccelerationStructureKHR, 1),
		vk::DescriptorPoolSize(vk::DescriptorType::eStorageImage, 2),
		vk::DescriptorPoolSize(vk::DescriptorType::eStorageBuffer, 5),
		vk::DescriptorPoolSize(vk::DescriptorType::eUniformBuffer, 1),
		vk::DescriptorPoolSize(vk::DescriptorType::eCombinedImageSampler, UINT32_MAX)
	};
	return context.GetDevice().createDescriptorPool(vk::DescriptorPoolCreateInfo(vk::DescriptorPoolCreateFlagBits::eUpdateAfterBind, 1, pool_sizes));
}

void RendererPipeline::Destroy(const DeviceContext& context) {
	vk::Device device = context.GetDevice();
	device.destroyDescriptorSetLayout(descriptor_set_layout);
	shader_binding_table->Destroy(context);
	device.destroyPipeline(pipeline);
	device.destroyPipelineLayout(pipeline_layout);
}
