#include "Application/Renderer/RenderContext.h"
#include "Core/DeviceContext.h"
#include "Engine/Components/Render.h"
#include "Engine/Components/Transform.h"
#include "Engine/Components/Camera.h"
#include "Core/Mesh.h"
#include "acceleration-structure.h"
#include <tuple>
#include "Engine/ShaderHostBuffer.h"
#include "Engine/Resources/ResourcesManager.h"
#include "Core/Image.h"
#include "Application/Renderer/CommandUtilities.h"


vk::TransformMatrixKHR RenderContext::GetTransformMatrixKHR(const glm::mat4 transform) {
	std::array<std::array<float, 4>, 3> matrix;
	for (size_t i = 0; i < 3; i++) {
		for (size_t j = 0; j < 4; j++) {
			matrix[i][j] = transform[j][i];
		}
	}
	return vk::TransformMatrixKHR(matrix);
}

RenderContext::RenderContext(const DeviceContext& context) {
	constants_data.sample_per_pixel = 0;
	// Constants Buffer
	{
		constants_buffer = Buffer(
			context,
			vk::BufferCreateInfo({}, sizeof(ConstantsData), vk::BufferUsageFlagBits::eUniformBuffer),
			vma::AllocationCreateInfo(vma::AllocationCreateFlagBits::eHostAccessSequentialWrite, vma::MemoryUsage::eAuto)
		);
		constants_buffer.SetName(context, "Constants Buffer");
	}
}

void RenderContext::UploadMeshes(const DeviceContext& context, entt::registry& registry) {
	auto view = registry.view<const LocalTransform, const MeshComponent, const MaterialComponent>();

	std::vector<MeshPool::UsedMesh> used_meshes;
	view.each([&used_meshes](const LocalTransform& transform, const MeshComponent& mesh, const MaterialComponent& material) {
		used_meshes.emplace_back(std::make_tuple(mesh.device_ids, mesh.resource_id));
	});
	mesh_pool.EnsureMeshes(context, used_meshes);
}

void RenderContext::UploadMaterials(
	const DeviceContext& context, 
	entt::registry& registry,
	const std::unordered_map<Uuid, uint32_t>& shader_indices
) {
	texture_pool.SetDirty(false);

	auto& host_shaders = registry.ctx().get<HostShaderManager&>();
	const auto& resource_manager = registry.ctx().get<const ResourcesManager&>();

	// update materials
	auto view = registry.view<const LocalTransform, const MeshComponent, const MaterialComponent>();
	MaterialVisiableContext material_context(texture_pool, host_shaders);
	std::unordered_set<Uuid> used_materials;
	view.each([&used_materials](const LocalTransform& transform, const MeshComponent& mesh, const MaterialComponent& material) {
		used_materials.insert(material.resource_ids.begin(), material.resource_ids.end());
	});
	for (const auto& material : used_materials) {
		const auto& resource = resource_manager.GetResource(material);
		const auto result = UpdateMaterialDataRegistry::Get(resource.GetResourceType());
		if (!result.has_value()) {
			throw std::runtime_error("Invalid mateiral");
		}
		result.value()(resource, material_context);
	}
	
	// update textures
	texture_pool.Ensure(context);

	// update shaders
	std::vector<std::tuple<const Uuid, const std::vector<std::byte>, const bool, const size_t>> data;
	for (const auto& shader_wrapper : host_shaders.GetAllShaders()) {
		const auto& shader = shader_wrapper.get();
		const auto& id = shader.GetId();
		if (!shader_indices.contains(id)) {
			throw std::runtime_error("Unuploaded shader: [" + id.str() + "]");
		}
		data.emplace_back(std::make_tuple(id, shader.GetData(), shader.IsDirty(), shader.GetStride()));
	}
	material_pool.EnsureBuffers(context, data);
}

void RenderContext::RecreateInstances(
	const DeviceContext& context, 
	entt::registry& registry,
	const std::unordered_map<Uuid, uint32_t>& shader_indices
) {
	auto view = registry.view<const LocalTransform, const MeshComponent, const MaterialComponent>();
	std::vector<vk::AccelerationStructureInstanceKHR> as_instances;
	std::vector<InstanceData> instances_data;
	uint32_t index = 0;

	const auto& host_shaders = registry.ctx().get<const HostShaderManager&>();
	view.each([
		&as_instances, 
		&index,
		&instances_data,
		&mesh_pool = this->mesh_pool, 
		&material_pool = this->material_pool,
		&host_shaders,
		&shader_indices
	](  const LocalTransform& transform,
		const MeshComponent& mesh, 
		const MaterialComponent& material
	) {
		for (size_t i = 0, end = mesh.device_ids.size(); i < end; i++) {
			const Mesh& device_mesh = mesh_pool.GetMesh(mesh.device_ids[i]);
			const auto& [shader_id, material_index] = host_shaders.GetMaterialRuntimeData(material.resource_ids[i]);
			as_instances.emplace_back(vk::AccelerationStructureInstanceKHR(
				GetTransformMatrixKHR(transform.matrix), index, 0xFF, shader_indices.at(shader_id),
				vk::GeometryInstanceFlagBitsKHR::eTriangleCullDisable,
				device_mesh.GetAsAddress()
			));

			InstanceData instance_data(
				device_mesh.GetVertexPositionAddress(),
				device_mesh.GetVertexOtherAddress(),
				device_mesh.GetIndexAddress(),
				material_pool.GetShaderBuffer(shader_id).GetDeviceAddress(),
				material_index
			);
			instances_data.emplace_back(instance_data);
			index++;
		}
	});

	// Recreate buffers
	context.GetDevice().destroyAccelerationStructureKHR(tlas);
	tlas_buffer.Destroy(context);
	tlas_instance_buffer.Destroy(context);
	instances_buffer.Destroy(context);

	if (as_instances.size() == 0) return;

	std::tie(tlas, tlas_buffer, tlas_instance_buffer) = BuildTlas(context, as_instances, {}, false);
	vk::BufferCreateInfo create_info({}, {}, vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eShaderDeviceAddress);
	instances_buffer = Buffer::CreateWithData<InstanceData>(context, create_info, instances_data);
	instances_buffer.SetName(context, "Instance Data Buffer");
}

void RenderContext::UpdatePushConstants(const DeviceContext& context, entt::registry& registry) {
	auto view = registry.view<const Camera>();
	view.each([&constants_data = this->constants_data, &registry](entt::entity entity, const Camera& camera) {
		if (registry.all_of<CameraDirtyTag>(entity)) {
			constants_data.sample_per_pixel = 0;
		}
		constants_data.projection_inverse = glm::inverse(camera.projection);
		constants_data.view_inverse = glm::inverse(camera.view);
	});

	constants_buffer.SetData<ConstantsData>(context, constants_data);
	constants_data.sample_per_pixel++;
}

void RenderContext::Update(
	const DeviceContext& context,
	entt::registry& registry, 
	const std::unordered_map<Uuid, uint32_t>& shader_indices
) {
	UploadMaterials(context, registry, shader_indices);
	UploadMeshes(context, registry);
	UpdatePushConstants(context, registry);
	RecreateInstances(context, registry, shader_indices);
}

void RenderContext::Destory(const DeviceContext& context) {
	context.GetDevice().destroyAccelerationStructureKHR(tlas);
	tlas_buffer.Destroy(context);
	tlas_instance_buffer.Destroy(context);

	mesh_pool.Destroy(context);
	texture_pool.Destroy(context);
	material_pool.Destroy(context);
	instances_buffer.Destroy(context);
	constants_buffer.Destroy(context);

	output_image->Destroy(context);
}

void RenderContext::RecreateOutputImage(const DeviceContext& context, const vk::Extent2D extent, vk::Format format) {
	constants_data.sample_per_pixel = 0;
	if (output_image) {
		output_image->Destroy(context);
	}
	output_image = std::make_unique<OutputImage>(context, extent, format);
}

RenderContext::OutputImage::OutputImage(const DeviceContext& context, const vk::Extent2D extent, vk::Format format) : 
	output_image(context, vk::ImageCreateInfo({}, vk::ImageType::e2D, format,
		vk::Extent3D(extent, 1), 1, 1, vk::SampleCountFlagBits::e1, vk::ImageTiling::eOptimal,
		vk::ImageUsageFlagBits::eStorage | vk::ImageUsageFlagBits::eTransferSrc)),
    accumulate_image(context, vk::ImageCreateInfo({}, vk::ImageType::e2D, vk::Format::eR32G32B32A32Sfloat, 
		vk::Extent3D(extent, 1), 1, 1, vk::SampleCountFlagBits::e1, vk::ImageTiling::eOptimal,
		vk::ImageUsageFlagBits::eStorage | vk::ImageUsageFlagBits::eTransferDst
	)) {

	vk::ImageSubresourceRange accumulate_image_subresource(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
	vk::CommandBuffer cmd = context.GetTempCmd();
	CommandUtilities::CmdInsertImageBarrier(cmd, vk::ImageMemoryBarrier(
		vk::AccessFlagBits::eNone, vk::AccessFlagBits::eShaderWrite,
		vk::ImageLayout::eUndefined, vk::ImageLayout::eGeneral,
		{}, {}, accumulate_image, accumulate_image_subresource));
	cmd.clearColorImage(accumulate_image, vk::ImageLayout::eGeneral, vk::ClearColorValue(0, 0, 0, 0),
		vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1));
	context.ReleaseTempCmd(cmd);

	output_image_view = context.GetDevice().createImageView(vk::ImageViewCreateInfo({}, output_image,
		vk::ImageViewType::e2D, format,
		vk::ComponentMapping(vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eG, vk::ComponentSwizzle::eB, vk::ComponentSwizzle::eA),
		vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1)
	));

	accumulate_image_view = context.GetDevice().createImageView(vk::ImageViewCreateInfo({}, accumulate_image,
		vk::ImageViewType::e2D, vk::Format::eR32G32B32A32Sfloat,
		vk::ComponentMapping(vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eG, vk::ComponentSwizzle::eB, vk::ComponentSwizzle::eA),
		vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1)
	));

	this->extent = extent;
}

void RenderContext::OutputImage::Destroy(const DeviceContext& context) {
	context.GetDevice().destroyImageView(output_image_view);
	context.GetDevice().destroyImageView(accumulate_image_view);
	output_image.Destroy(context);
	accumulate_image.Destroy(context);
}
