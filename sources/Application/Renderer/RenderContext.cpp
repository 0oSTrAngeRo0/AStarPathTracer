#include "Application/Renderer/RenderContext.h"
#include "Core/DeviceContext.h"
#include "Engine/Components/Render.h"
#include "Engine/Components/Transform.h"
#include "Engine/Components/Camera.h"
#include "Core/Mesh.h"
#include "acceleration-structure.h"
#include <tuple>
#include "Engine/ShaderHostBuffer.h"


vk::TransformMatrixKHR GetTransformMatrixKHR(const glm::mat4 transform) {
	std::array<std::array<float, 4>, 3> matrix;
	for (size_t i = 0; i < 3; i++) {
		for (size_t j = 0; j < 4; j++) {
			matrix[i][j] = transform[j][i];
		}
	}
	return vk::TransformMatrixKHR(matrix);
}

RenderContext::RenderContext(const DeviceContext& context) {
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

	std::vector<std::tuple<Uuid, Uuid>> used_meshes;
	view.each([&used_meshes](const LocalTransform& transform, const MeshComponent& mesh, const MaterialComponent& material) {
		used_meshes.emplace_back(std::make_tuple(mesh.device_id, mesh.resource_id));
		});
	mesh_pool.EnsureMeshes(context, used_meshes);
}

void RenderContext::UploadMaterials(const DeviceContext& context, entt::registry& registry) {
	auto view = registry.view<const LocalTransform, const MeshComponent, const MaterialComponent>();
	std::vector<Uuid> used_materials;
	view.each([&used_materials](const LocalTransform& transform, const MeshComponent& mesh, const MaterialComponent& material) {
		used_materials.emplace_back(material.resource_id);
	});
	material_pool.EnsureMaterials(used_materials);

	const auto& host_shaders = registry.ctx().get<const HostShaderManager&>();
	std::vector<std::tuple<const Uuid, const std::vector<std::byte>, const bool>> data;
	for (const auto& shader_wrapper : host_shaders.GetAllShaders()) {
		const auto& shader = shader_wrapper.get();
		data.emplace_back(std::make_tuple(shader.GetId(), shader.GetData(), shader.IsDirty()));
	}
	material_pool.EnsureBuffers(context, data);
}

void RenderContext::RecreateInstances(const DeviceContext& context, entt::registry& registry) {
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
		&host_shaders
	](  const LocalTransform& transform,
		const MeshComponent& mesh, 
		const MaterialComponent& material
	) {
		const Mesh& device_mesh = mesh_pool.GetMesh(mesh.device_id);
		const auto& [shader_id, material_index] = host_shaders.GetMaterialRuntimeData(material.resource_id);
		as_instances.emplace_back(vk::AccelerationStructureInstanceKHR(
			GetTransformMatrixKHR(transform.matrix), index, 0xFF, 0,
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
	ConstantsData data;
	registry.view<const Camera>().each([&data](const Camera& camera) {
		data.projection_inverse = glm::inverse(camera.projection);
		data.view_inverse = glm::inverse(camera.view);
	});
	constants_buffer.SetData<ConstantsData>(context, data);
}

void RenderContext::Update(const DeviceContext& context, entt::registry& registry) {
	UploadMaterials(context, registry);
	UploadMeshes(context, registry);
	UpdatePushConstants(context, registry);
	RecreateInstances(context, registry);
}

void RenderContext::Destory(const DeviceContext& context) {
	context.GetDevice().destroyAccelerationStructureKHR(tlas);
	tlas_buffer.Destroy(context);
	tlas_instance_buffer.Destroy(context);

	mesh_pool.Destroy(context);
	material_pool.Destroy(context);
	instances_buffer.Destroy(context);
	constants_buffer.Destroy(context);
}
