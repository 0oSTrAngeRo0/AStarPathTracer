#include "Application/Renderer/Renderer.h"
#include "acceleration-structure.h"
#include "Application/Renderer/RenderContext.h"
#include <stdexcept>
#include "Application/Renderer/Renderer.h"
#include "Core/VulkanUtils.h"
#include "Engine/Components/Camera.h"
#include "Engine/Components/Transform.h"
#include "Engine/Components/Render.h"
#include "Application/Renderer/RenderContext.h"
#include "Application/Renderer/Renderer.h"
#include "file-system.h"
#include "Core/VulkanUtils.h"
#include "math-utils.h"
#include "acceleration-structure.h"
#include "Application/Renderer/RenderContext.h"
#include "Engine/Resources/ResourcesManager.h"
#include <Engine/Resources/ResourceData.h>

Renderer::Renderer(const DeviceContext& context, const RenderContext& render) {
	swapchain = std::make_unique<Swapchain>(context);
	rt_image = std::make_unique<Image>(context, vk::ImageCreateInfo(
		{}, vk::ImageType::e2D, context.GetSurfaceFormat().format,
		vk::Extent3D(swapchain->GetExtent(), 1), 1, 1, vk::SampleCountFlagBits::e1, vk::ImageTiling::eOptimal,
		vk::ImageUsageFlagBits::eStorage | vk::ImageUsageFlagBits::eTransferSrc));
	rt_image_view = context.GetDevice().createImageView(vk::ImageViewCreateInfo({}, *rt_image,
		vk::ImageViewType::e2D, context.GetSurfaceFormat().format,
		vk::ComponentMapping(vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eG, vk::ComponentSwizzle::eB, vk::ComponentSwizzle::eA),
		vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1)
	));

	UploadDescriptorSet(context, render);
	CreateRayTracingPipelineLayout(context);
	CreatePipelineAndBindingTable(context);
	command_pool = context.GetDevice().createCommandPool(vk::CommandPoolCreateInfo(vk::CommandPoolCreateFlagBits::eResetCommandBuffer, context.GetGrpahicsQueueIndex()));
	CreateSyncObjects(context);
}

void Renderer::CmdInsertImageBarrier(const vk::CommandBuffer cmd, vk::ImageMemoryBarrier barrier) {
	barrier.srcQueueFamilyIndex = vk::QueueFamilyIgnored;
	barrier.dstQueueFamilyIndex = vk::QueueFamilyIgnored;
	cmd.pipelineBarrier(vk::PipelineStageFlagBits::eAllCommands, vk::PipelineStageFlagBits::eAllCommands, {}, {}, {}, barrier);
}

void Renderer::UpdateDescriptorSet(const DeviceContext& context, const RenderContext& render, const vk::DescriptorSet& set) {
	std::vector<vk::AccelerationStructureKHR> tlases = { render.GetTlas() };
	vk::WriteDescriptorSetAccelerationStructureKHR write_tlas(tlases);
	vk::DescriptorBufferInfo write_vertex_position_buffer(render.GetVertexPositionBuffer(), 0, vk::WholeSize);
	vk::DescriptorBufferInfo write_vertex_other_buffer(render.GetVertexOtherBuffer(), 0, vk::WholeSize);
	vk::DescriptorBufferInfo write_index_buffer(render.GetIndexBuffer(), 0, vk::WholeSize);
	vk::DescriptorBufferInfo write_material_buffer(render.GetMaterialBuffer(), 0, vk::WholeSize);
	vk::DescriptorBufferInfo write_instance_buffer(render.GetInstancesBuffer(), 0, vk::WholeSize);
	vk::DescriptorBufferInfo write_constants_buffer(render.GetConstantsBuffer(), 0, vk::WholeSize);
	std::vector<vk::WriteDescriptorSet> writes = {
		vk::WriteDescriptorSet(set, 0, 0, 1, vk::DescriptorType::eAccelerationStructureKHR, {}, {}, {}, &write_tlas),
		vk::WriteDescriptorSet(set, 2, 0, vk::DescriptorType::eStorageBuffer, {}, write_vertex_position_buffer),
		vk::WriteDescriptorSet(set, 3, 0, vk::DescriptorType::eStorageBuffer, {}, write_vertex_other_buffer),
		vk::WriteDescriptorSet(set, 4, 0, vk::DescriptorType::eStorageBuffer, {}, write_index_buffer),
		vk::WriteDescriptorSet(set, 5, 0, vk::DescriptorType::eStorageBuffer, {}, write_material_buffer),
		vk::WriteDescriptorSet(set, 6, 0, vk::DescriptorType::eStorageBuffer, {}, write_instance_buffer),
		vk::WriteDescriptorSet(set, 7, 0, vk::DescriptorType::eUniformBuffer, {}, write_constants_buffer),
	};
	context.GetDevice().updateDescriptorSets(writes, {});
}

vk::CommandBuffer Renderer::CreateFrameCommandBuffer(const DeviceContext& context, const vk::CommandPool pool, const uint32_t index) {
	vk::Device device = context.GetDevice();
	vk::CommandBufferAllocateInfo allocate_info(pool, vk::CommandBufferLevel::ePrimary, 1);
	vk::CommandBuffer cmd = device.allocateCommandBuffers(allocate_info)[0];

	vk::ImageSubresourceRange subresource(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
	vk::ImageCopy copy_region(
		vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, 0, 0, 1),
		vk::Offset3D(0, 0, 0),
		vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, 0, 0, 1),
		vk::Offset3D(0, 0, 0),
		vk::Extent3D(swapchain->GetExtent(), 1)
	);

	vk::Image target = swapchain->GetImage(index);

	cmd.begin(vk::CommandBufferBeginInfo());
	CmdInsertImageBarrier(cmd, vk::ImageMemoryBarrier(
		vk::AccessFlagBits::eNone, vk::AccessFlagBits::eShaderWrite,
		vk::ImageLayout::eUndefined, vk::ImageLayout::eGeneral,
		{}, {}, *rt_image, subresource));
	cmd.bindPipeline(vk::PipelineBindPoint::eRayTracingKHR, pipeline);
	cmd.bindDescriptorSets(vk::PipelineBindPoint::eRayTracingKHR, pipeline_layout, 0, vk::ArrayProxy(descriptor_set), {});
	cmd.traceRaysKHR(
		shader_binding_table->GetRayGen(),
		shader_binding_table->GetMiss(),
		shader_binding_table->GetClosestHit(),
		shader_binding_table->GetCallable(),
		swapchain->GetExtent().width,
		swapchain->GetExtent().height,
		1
	);
	CmdInsertImageBarrier(cmd, vk::ImageMemoryBarrier(
		vk::AccessFlagBits::eNone, vk::AccessFlagBits::eShaderWrite,
		vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal,
		{}, {}, target, subresource));
	CmdInsertImageBarrier(cmd, vk::ImageMemoryBarrier(
		vk::AccessFlagBits::eShaderWrite, vk::AccessFlagBits::eShaderRead,
		vk::ImageLayout::eGeneral, vk::ImageLayout::eTransferSrcOptimal,
		{}, {}, *rt_image, subresource));
	cmd.copyImage(*rt_image, vk::ImageLayout::eTransferSrcOptimal, target, vk::ImageLayout::eTransferDstOptimal, copy_region);
	CmdInsertImageBarrier(cmd, vk::ImageMemoryBarrier(
		vk::AccessFlagBits::eNone, vk::AccessFlagBits::eShaderWrite,
		vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::ePresentSrcKHR,
		{}, {}, target, subresource));
	cmd.end();
	return cmd;
}

void Renderer::CreateSyncObjects(const DeviceContext& context) {
	vk::SemaphoreCreateInfo semaphore_create_info;
	vk::FenceCreateInfo fence_create_info(vk::FenceCreateFlagBits::eSignaled);
	image_available_semaphore = context.GetDevice().createSemaphore(semaphore_create_info);
	render_finished_semaphore = context.GetDevice().createSemaphore(semaphore_create_info);
	in_flight_fence = context.GetDevice().createFence(fence_create_info);
}

void Renderer::Draw(const DeviceContext& context, const RenderContext& render) {
	VK_CHECK(context.GetDevice().waitForFences({ in_flight_fence }, vk::True, UINT64_MAX));
	context.GetDevice().resetFences({ in_flight_fence });

	UpdateDescriptorSet(context, render, descriptor_set);
	uint32_t image_index = context.GetDevice().acquireNextImageKHR(*swapchain, UINT64_MAX, image_available_semaphore).value;
	vk::CommandBuffer cmd = CreateFrameCommandBuffer(context, command_pool, image_index);

	vk::PipelineStageFlags stage_mask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
	vk::SubmitInfo submit_info(
		image_available_semaphore,
		stage_mask,
		cmd,
		render_finished_semaphore
	);
	context.GetGraphicsQueue().submit(submit_info, in_flight_fence);
	context.GetGraphicsQueue().waitIdle(); // 等待当前帧执行完毕， TODO: 优化之
	context.GetDevice().freeCommandBuffers(command_pool, cmd);
	std::vector<vk::SwapchainKHR> swapchains = { *swapchain };
	vk::PresentInfoKHR present_info(render_finished_semaphore, swapchains, image_index);
	VK_CHECK(context.GetPresentQueue().presentKHR(present_info));
}

vk::DescriptorPool Renderer::CreateDescriptorPool(const vk::Device device) {
	std::vector<vk::DescriptorPoolSize> pool_sizes = {
		vk::DescriptorPoolSize(vk::DescriptorType::eAccelerationStructureKHR, 1),
		vk::DescriptorPoolSize(vk::DescriptorType::eStorageImage, 1),
		vk::DescriptorPoolSize(vk::DescriptorType::eStorageBuffer, 1),
		vk::DescriptorPoolSize(vk::DescriptorType::eStorageBuffer, 1),
		vk::DescriptorPoolSize(vk::DescriptorType::eStorageBuffer, 1),
		vk::DescriptorPoolSize(vk::DescriptorType::eStorageBuffer, 1),
		vk::DescriptorPoolSize(vk::DescriptorType::eStorageBuffer, 1),
		vk::DescriptorPoolSize(vk::DescriptorType::eUniformBuffer, 1)
	};
	return device.createDescriptorPool(vk::DescriptorPoolCreateInfo({}, 1, pool_sizes));
}

vk::DescriptorSetLayout Renderer::CreateDescriptorSetLayout(const vk::Device device) {
	std::vector<vk::DescriptorSetLayoutBinding> bindings = {
		vk::DescriptorSetLayoutBinding(0, vk::DescriptorType::eAccelerationStructureKHR, 1, vk::ShaderStageFlagBits::eRaygenKHR), // tlas
		vk::DescriptorSetLayoutBinding(1, vk::DescriptorType::eStorageImage, 1, vk::ShaderStageFlagBits::eRaygenKHR), // output image
		vk::DescriptorSetLayoutBinding(2, vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eClosestHitKHR), // vertex position buffer
		vk::DescriptorSetLayoutBinding(3, vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eClosestHitKHR), // vertex other buffer
		vk::DescriptorSetLayoutBinding(4, vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eClosestHitKHR), // index buffer
		vk::DescriptorSetLayoutBinding(5, vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eClosestHitKHR), // material buffer
		vk::DescriptorSetLayoutBinding(6, vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eClosestHitKHR), // instance buffer
		vk::DescriptorSetLayoutBinding(7, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eRaygenKHR) // constants buffer
	};
	return device.createDescriptorSetLayout(vk::DescriptorSetLayoutCreateInfo({}, bindings));
}

void Renderer::UploadDescriptorSet(const DeviceContext& context, const RenderContext& render) {
	descriptor_pool = CreateDescriptorPool(context.GetDevice());
	descriptor_set_layout = CreateDescriptorSetLayout(context.GetDevice());
	std::vector<vk::DescriptorSet> sets = context.GetDevice().allocateDescriptorSets(vk::DescriptorSetAllocateInfo(descriptor_pool, descriptor_set_layout));
	descriptor_set = sets[0];

	std::vector<vk::AccelerationStructureKHR> tlases = { render.GetTlas() };
	vk::WriteDescriptorSetAccelerationStructureKHR write_tlas(tlases);
	vk::DescriptorImageInfo write_rt_image({}, rt_image_view, vk::ImageLayout::eGeneral);
	vk::DescriptorBufferInfo write_vertex_position_buffer(render.GetVertexPositionBuffer(), 0, vk::WholeSize);
	vk::DescriptorBufferInfo write_vertex_other_buffer(render.GetVertexOtherBuffer(), 0, vk::WholeSize);
	vk::DescriptorBufferInfo write_index_buffer(render.GetIndexBuffer(), 0, vk::WholeSize);
	vk::DescriptorBufferInfo write_material_buffer(render.GetMaterialBuffer(), 0, vk::WholeSize);
	vk::DescriptorBufferInfo write_instance_buffer(render.GetInstancesBuffer(), 0, vk::WholeSize);
	vk::DescriptorBufferInfo write_constants_buffer(render.GetConstantsBuffer(), 0, vk::WholeSize);
	std::vector<vk::WriteDescriptorSet> writes = {
		vk::WriteDescriptorSet(descriptor_set, 0, 0, 1, vk::DescriptorType::eAccelerationStructureKHR, {}, {}, {}, &write_tlas),
		vk::WriteDescriptorSet(descriptor_set, 1, 0, vk::DescriptorType::eStorageImage, write_rt_image),
		vk::WriteDescriptorSet(descriptor_set, 2, 0, vk::DescriptorType::eStorageBuffer, {}, write_vertex_position_buffer),
		vk::WriteDescriptorSet(descriptor_set, 3, 0, vk::DescriptorType::eStorageBuffer, {}, write_vertex_other_buffer),
		vk::WriteDescriptorSet(descriptor_set, 4, 0, vk::DescriptorType::eStorageBuffer, {}, write_index_buffer),
		vk::WriteDescriptorSet(descriptor_set, 5, 0, vk::DescriptorType::eStorageBuffer, {}, write_material_buffer),
		vk::WriteDescriptorSet(descriptor_set, 6, 0, vk::DescriptorType::eStorageBuffer, {}, write_instance_buffer),
		vk::WriteDescriptorSet(descriptor_set, 7, 0, vk::DescriptorType::eUniformBuffer, {}, write_constants_buffer),
	};
	context.GetDevice().updateDescriptorSets(writes, {});
}

void Renderer::CreateRayTracingPipelineLayout(const DeviceContext& context) {
	vk::PipelineLayoutCreateInfo create_info({}, descriptor_set_layout);
	pipeline_layout = context.GetDevice().createPipelineLayout(create_info);
}

void Renderer::CreatePipelineAndBindingTable(const DeviceContext& context) {
	std::vector<RayTracingShaders::ShaderData> shaders;
	ResourcesManager::GetInstance().IterateResources([&shaders](const std::string& path, const ResourceBase& resource) {
		if (resource.GetResourceType() != Resource<ShaderResourceData>::GetResourceTypeStatic()) return;
		const ShaderResourceData& shader = static_cast<const Resource<ShaderResourceData>&>(resource).resource_data;
		shaders.emplace_back(RayTracingShaders::ShaderData(shader.compiled_code_path, shader.entry_function, shader.shader_stage));
	});

	RayTracingShaders::PipelineData pipeline_data(context, shaders);
	vk::RayTracingPipelineCreateInfoKHR create_info({}, pipeline_data.GetStages(), pipeline_data.GetGroups(), 1, {}, {}, {}, pipeline_layout);
	pipeline = context.GetDevice().createRayTracingPipelineKHR({}, {}, create_info).value;

	shader_binding_table = std::make_unique<RayTracingShaders::BindingTable>(context, pipeline, pipeline_data.GetShaderCount());

	pipeline_data.Destroy(context);
}

void Renderer::Destroy(const DeviceContext& context) {
	vk::Device device = context.GetDevice();
	device.waitIdle();
	device.destroySemaphore(image_available_semaphore);
	device.destroySemaphore(render_finished_semaphore);
	device.destroyFence(in_flight_fence);
	device.destroyCommandPool(command_pool);
	device.destroyImageView(rt_image_view);
	rt_image->Destroy(context);
	device.destroyDescriptorSetLayout(descriptor_set_layout);
	device.destroyDescriptorPool(descriptor_pool);
	shader_binding_table->Destroy(context);
	device.destroyPipeline(pipeline);
	device.destroyPipelineLayout(pipeline_layout);
	swapchain->Destroy(context);
}
