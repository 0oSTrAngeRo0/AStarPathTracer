#pragma once

#include "Core/Buffer.h"
#include <glm/glm.hpp>
#include "Core/DeviceContext.h"
#include "Core/Shader.h"
#include "Core/Object.h"
#include "Core/ModelLoader/ObjLoader.h"
#include "acceleration-structure.h"

struct LitMaterialData
{
	glm::vec4 color;
};

class Scene {
public:
	std::vector<std::shared_ptr<ShaderBase>> shaders;
	std::vector<Object> objects;
	std::vector<std::shared_ptr<Mesh>> meshes;

	Buffer material_buffer;
	Buffer vertex_buffer;
	Buffer instance_buffer;
	Buffer index_buffer;

	vk::AccelerationStructureKHR tlas;
	Buffer tlas_buffer;
	Buffer tlas_instance_buffer;

	Scene(const DeviceContext& context) {
		std::shared_ptr<Shader<LitMaterialData>> shader = std::make_shared<Shader<LitMaterialData>>();
		shaders.emplace_back(shader);

		std::shared_ptr<Mesh> mesh = LoadFromFile("D:/C++/Projects/PathTracer/models/cube.obj");
		mesh->UploadData(context);
		meshes.emplace_back(mesh);

		Material material = shader->CreateMaterial();
		LitMaterialData& material_data = material.GetData<LitMaterialData>();
		material_data.color = glm::vec4(0.3, 0.7, 0.3, 1.0);

		Object obj(material, mesh);
		objects.emplace_back(obj);

		shader->UpdateData(context);

		std::tie(tlas, tlas_buffer, tlas_instance_buffer) = CreateAs(context, meshes, objects);

		CreateBuffers(context);
	}

	void CreateBuffers(const DeviceContext& context) {
		{
			std::vector<vk::DeviceAddress> data;
			for (const auto& shader : shaders) {
				data.emplace_back(shader->GetBufferAddress());
			}
			vk::BufferCreateInfo create_info({}, {}, vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eShaderDeviceAddress);
			material_buffer = Buffer::CreateWithData<vk::DeviceAddress>(context, create_info, data);
		}

		{
			std::vector<vk::DeviceAddress> data;
			for (const auto& mesh : meshes) {
				data.emplace_back(mesh->GetVertexAddress());
			}
			vk::BufferCreateInfo create_info({}, {}, vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eShaderDeviceAddress);
			vertex_buffer = Buffer::CreateWithData<vk::DeviceAddress>(context, create_info, data);
		}

		{
			std::vector<vk::DeviceAddress> data;
			for (const auto& mesh : meshes) {
				data.emplace_back(mesh->GetIndexAddress());
			}
			vk::BufferCreateInfo create_info({}, {}, vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eShaderDeviceAddress);
			index_buffer = Buffer::CreateWithData<vk::DeviceAddress>(context, create_info, data);
		}

		{
			std::vector<InstanceData> data;
			for (const Object& obj : objects) {
				data.emplace_back(obj.GetInstanceData());
			}
			vk::BufferCreateInfo create_info({}, {}, vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eShaderDeviceAddress);
			instance_buffer = Buffer::CreateWithData<InstanceData>(context, create_info, data);
		}
	}

	void Destory(const DeviceContext& context) {
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
		instance_buffer.Destroy(context);
		index_buffer.Destroy(context);
	}
};