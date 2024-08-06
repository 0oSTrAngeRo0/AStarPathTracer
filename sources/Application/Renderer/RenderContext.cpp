#include "Application/Renderer/RenderContext.h"
#include "Core/DeviceContext.h"
#include "Engine/Components/Render.h"
#include "Engine/Components/Transform.h"
#include "Engine/Components/Camera.h"
#include "Core/Mesh.h"
#include "acceleration-structure.h"
#include <tuple>

vk::TransformMatrixKHR GetTransformMatrixKHR(const glm::mat4 transform) {
	std::array<std::array<float, 4>, 3> matrix;
	for (size_t i = 0; i < 3; i++) {
		for (size_t j = 0; j < 4; j++) {
			matrix[i][j] = transform[j][i];
		}
	}
	return vk::TransformMatrixKHR(matrix);
}

std::vector<std::shared_ptr<Mesh>> CreateMeshes(const DeviceContext& context) {
	std::vector<std::shared_ptr<Mesh>> meshes;
	return meshes;
}

RenderContext::RenderContext(const DeviceContext& context) {
	std::shared_ptr<Shader<LitMaterialData>> shader = std::make_shared<Shader<LitMaterialData>>();

	// create sample material data for test
	std::shared_ptr<MaterialTemplate<LitMaterialData>> material = std::static_pointer_cast<MaterialTemplate<LitMaterialData>>(shader->CreateMaterial());
	LitMaterialData material_data = material->GetData();
	material_data.color = glm::vec4(0.3, 0.3, 0.7, 1.0);
	material->SetData(material_data);

	shaders.emplace_back(shader);

	// Material Address Buffer
	{
		std::vector<vk::DeviceAddress> data;
		for (const auto& shader : shaders) {
			shader->CreateMaterial();
			shader->UpdateData(context);
			data.emplace_back(shader->GetBufferAddress());
		}
		vk::BufferCreateInfo create_info({}, {}, vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eShaderDeviceAddress);
		material_buffer = Buffer::CreateWithData<vk::DeviceAddress>(context, create_info, data);
		material_buffer.SetName(context, "Material Address Buffer");
	}

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

void RenderContext::RecreateInstances(const DeviceContext& context, entt::registry& registry) {
	// Collect data
	auto view = registry.view<const LocalTransform, const MeshComponent, const MaterialComponent>();

	std::vector<std::tuple<Uuid, Uuid>> used_meshes;
	view.each([&used_meshes](const LocalTransform& transform, const MeshComponent& mesh, const MaterialComponent& material) {
		used_meshes.emplace_back(std::make_tuple(mesh.id, mesh.resource));
	});
	mesh_pool.EnsureMeshes(context, used_meshes);

	std::vector<vk::AccelerationStructureInstanceKHR> as_instances;
	std::vector<InstanceData> instances_data;
	uint32_t index = 0;

	view.each([&as_instances, &index, &instances_data, &mesh_pool = this->mesh_pool, &shaders = this->shaders](const LocalTransform& transform, const MeshComponent& mesh, const MaterialComponent& material) {
		const Mesh& device_mesh = mesh_pool.GetMesh(mesh.id);
		as_instances.emplace_back(vk::AccelerationStructureInstanceKHR(
			GetTransformMatrixKHR(transform.matrix), index, 0xFF, 0,
			vk::GeometryInstanceFlagBitsKHR::eTriangleCullDisable,
			device_mesh.GetAsAddress()
		));

		InstanceData instance_data(
			device_mesh.GetVertexAddress(),
			device_mesh.GetIndexAddress(),
			shaders[0]->GetBufferAddress(),
			0
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

void RenderContext::RecreateShaderBuffers(const DeviceContext& context) {
	for (auto shader : shaders) {
		shader->UpdateData(context);
	}
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
	RecreateShaderBuffers(context);
	UpdatePushConstants(context, registry);
	RecreateInstances(context, registry);
}

void RenderContext::Destory(const DeviceContext& context) {
	context.GetDevice().destroyAccelerationStructureKHR(tlas);
	tlas_buffer.Destroy(context);
	tlas_instance_buffer.Destroy(context);

	for (auto& shader : shaders) {
		shader->Destroy(context);
	}

	mesh_pool.Destroy(context);
	material_buffer.Destroy(context);
	instances_buffer.Destroy(context);
	constants_buffer.Destroy(context);
}
