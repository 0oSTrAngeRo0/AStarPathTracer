#include "Application/Renderer/RenderContext.h"
#include "Core/DeviceContext.h"
#include "Core/ModelLoader/ObjLoader.h"
#include "Engine/Components/Render.h"
#include "Engine/Components/Transform.h"
#include "Engine/Components/Camera.h"
#include "acceleration-structure.h"

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
	std::vector<std::string> paths = {
		"D:/C++/Projects/PathTracer/models/cube.obj"
	};
	std::vector<std::shared_ptr<Mesh>> meshes;
	for (const std::string& path : paths) {
		auto [positions, indices] = LoadFromFile(path);
		size_t vertex_count = positions.size();
		std::vector<Vertex> vertices;
		for (size_t i = 0; i < vertex_count; i++) {
			vertices.emplace_back(Vertex(positions[i]));
		}
		std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(context, vertices, indices);
		mesh->SetName(context, path);
		meshes.emplace_back(mesh);
	}
	return meshes;
}

RenderContext::RenderContext(const DeviceContext& context) {
	meshes = CreateMeshes(context);
	CreateBlases(context, meshes);

	std::shared_ptr<Shader<LitMaterialData>> shader = std::make_shared<Shader<LitMaterialData>>();
	shaders.emplace_back(shader);

	// Vertex Address Buffer
	{
		std::vector<vk::DeviceAddress> data;
		for (const auto& mesh : meshes) {
			data.emplace_back(mesh->GetVertexAddress());
		}
		vk::BufferCreateInfo create_info({}, {}, vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eShaderDeviceAddress);
		vertex_buffer = Buffer::CreateWithData<vk::DeviceAddress>(context, create_info, data);
		vertex_buffer.SetName(context, "Vertex Address Buffer");
	}

	// Index Address Buffer
	{
		std::vector<vk::DeviceAddress> data;
		for (const auto& mesh : meshes) {
			data.emplace_back(mesh->GetIndexAddress());
		}
		vk::BufferCreateInfo create_info({}, {}, vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eShaderDeviceAddress);
		index_buffer = Buffer::CreateWithData<vk::DeviceAddress>(context, create_info, data);
		index_buffer.SetName(context, "Index Address Buffer");
	}

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
	auto view = registry.view<const LocalTransform, const Renderable>();
	std::vector<vk::AccelerationStructureInstanceKHR> as_instances(instances.GetDataCount());

	view.each([&as_instances](const LocalTransform& transform, const Renderable& render) {
		uint32_t index =  render.instance->GetIndex();
		
  		as_instances[index] = vk::AccelerationStructureInstanceKHR(
			GetTransformMatrixKHR(transform.matrix), index, 0xFF, 0,
			vk::GeometryInstanceFlagBitsKHR::eTriangleCullDisable,
			render.mesh->GetAsAddress()
		);

		InstanceData instance_data(
			render.mesh->GetIndexAddress(),
			render.mesh->GetVertexAddress(),
			render.material->GetBufferAddress(),
			render.material->GetIndex()
		);
		render.instance->SetData(instance_data);
	});

	// Recreate buffers
	context.GetDevice().destroyAccelerationStructureKHR(tlas);
	tlas_buffer.Destroy(context);
	tlas_instance_buffer.Destroy(context);

	std::tie(tlas, tlas_buffer, tlas_instance_buffer) = BuildTlas(context, as_instances, {}, false);
	instances.UpdateData(context);

	Buffer buffer = instances.GetBuffer();
	// buffer.SetName(context, "Instance Data Buffer");
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

	for (auto& mesh : meshes) {
		mesh->Destroy(context);
	}

	material_buffer.Destroy(context);
	vertex_buffer.Destroy(context);
	instances.Destroy(context);
	index_buffer.Destroy(context);
	constants_buffer.Destroy(context);
}
