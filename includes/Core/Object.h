#pragma once

#include "Core/DeviceContext.h"
#include "Core/Shader.h"
#include "Core/Mesh.h"
#include "glm/glm.hpp"

struct InstanceData
{
	vk::DeviceAddress vertex_address;
	vk::DeviceAddress index_address;
	vk::DeviceAddress material_address;
	uint16_t material_index;
};

class Object {
public:
	Material material;
	std::shared_ptr<Mesh> mesh;
	glm::mat4x4 transform;

	Object(Material material, std::shared_ptr<Mesh> mesh) : material(material), mesh(mesh) {
		transform = glm::mat4x4(
			1.0, 0.0, 0.0, 0.0,
			0.0, 1.0, 0.0, 0.0,
			0.0, 0.0, 1.0, 0.0,
			0.0, 0.0, 0.0, 0.0
		);
	}

	inline glm::mat4x4 GetWorldTransform() const { return transform; }
	inline vk::TransformMatrixKHR GetTransformMatrixKHR() const {
		std::array<std::array<float, 4>, 3> matrix;
		for (size_t i = 0; i < 3; i++)
		{
			for (size_t j = 0; j < 4; j++) {
				matrix[i][j] = transform[i][j];
			}
		}
		return vk::TransformMatrixKHR(matrix);
	}

	InstanceData GetInstanceData() const {
		InstanceData data;
		data.vertex_address = mesh->GetVertexAddress();
		data.index_address = mesh->GetIndexAddress();
		data.material_address = material.GetBufferAddress();
		data.material_index = material.GetIndex();
		return data;
	}
};