#pragma once

#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
import Core:Buffer;
#include "Core/VulkanUsages.h"
#include <glm/glm.hpp>

class DeviceContext;

class Mesh {
public:
	using Index = glm::uvec3;
	using VertexPosition = glm::vec3;
	Mesh(
		const DeviceContext& context,
		const std::vector<glm::vec3>& positions, // vertex positions
		const std::vector<glm::vec3>& normals, // per vertex normal
		const std::vector<glm::vec4>& tangents, // per vertex tangent, glm::vec3 bitangent = cross(normal, tangent.xyz) * tangent.w
		const std::vector<glm::vec2>& uvs, // per vertex uv
		const std::vector<glm::uvec3>& indices // triangles
	);
	void Destroy(const DeviceContext& context);
	void SetName(const DeviceContext& context, const std::string& name);

	inline vk::Format GetVertexFormat() const { return vk::Format::eR32G32B32Sfloat; }
	inline vk::IndexType GetIndexFormat() const { return vk::IndexType::eUint32; }
	inline vk::DeviceAddress GetVertexPositionAddress() const { return vertex_position_buffer.GetDeviceAddress(); }
	inline vk::DeviceAddress GetVertexOtherAddress() const { return vertex_other_buffer.GetDeviceAddress(); }
	inline vk::DeviceAddress GetIndexAddress() const { return index_buffer.GetDeviceAddress(); }
	inline vk::DeviceSize GetVertexPositionStride() const { return sizeof(VertexPosition); }
	inline vk::DeviceSize GetVertexOtherStride() const { return sizeof(VertexOther); }
	inline vk::DeviceAddress GetAsAddress() const { return as_buffer.GetDeviceAddress(); }
	inline void SetAsData(vk::AccelerationStructureKHR blas, Buffer buffer) {
		this->as_buffer = buffer;
		this->blas = blas;
	}
	inline uint64_t GetTrianglesCount() const { return index_buffer.GetSize() / sizeof(Index); }
	inline uint64_t GetVertexCount() const { return vertex_position_buffer.GetSize() / sizeof(VertexPosition); }
private:
	struct VertexOther {
		glm::vec3 normal;
		glm::vec4 tangent;
		glm::vec2 uv;
		VertexOther(
			const glm::vec3& normal, 
			const glm::vec4& tagnent, 
			const glm::vec2& uv
		) : normal(normal), tangent(tagnent), uv(uv) {}
	};

	Buffer vertex_position_buffer;
	Buffer vertex_other_buffer;
	Buffer index_buffer;
	vk::AccelerationStructureKHR blas;
	Buffer as_buffer;
};;