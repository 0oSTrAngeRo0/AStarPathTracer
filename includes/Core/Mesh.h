#pragma once

#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1

#include "Core/VulkanUsages.h"
#include "Core/Buffer.h"
#include <glm/glm.hpp>

class DeviceContext;

class Mesh {
public:
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

	inline vk::DeviceAddress GetVertexAddress() const { return vertex_buffer.GetDeviceAddress(); }
	inline vk::DeviceAddress GetIndexAddress() const { return index_buffer.GetDeviceAddress(); }
	inline vk::DeviceSize GetVertexStride() const { return sizeof(Vertex); }
	inline vk::DeviceAddress GetAsAddress() const { return as_buffer.GetDeviceAddress(); }
	inline void SetAsData(vk::AccelerationStructureKHR blas, Buffer buffer) {
		this->as_buffer = buffer;
		this->blas = blas;
	}
	inline uint64_t GetTrianglesCount() const { return index_buffer.GetSize() / (3 * sizeof(uint32_t)); }
	inline uint64_t GetVertexCount() const { return vertex_buffer.GetSize() / sizeof(Vertex); }
private:
	struct Vertex {
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec4 tangent;
		glm::vec2 uv;
		Vertex(
			const glm::vec3& position, 
			const glm::vec3& normal, 
			const glm::vec4& tagnent, 
			const glm::vec2& uv
		) : position(position), normal(normal), tangent(tagnent), uv(uv) {}
	};

	Buffer vertex_buffer;
	Buffer index_buffer;
	vk::AccelerationStructureKHR blas;
	Buffer as_buffer;
};;