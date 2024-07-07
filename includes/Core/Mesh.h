#pragma once

#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1

#include "Core/VulkanUsages.h"
#include "Core/Buffer.h"
#include <glm/glm.hpp>

class DeviceContext;

struct Vertex {
	glm::vec3 position;

	Vertex(const glm::vec3& position) : position(position) {}
};

class Mesh {
public:
	Mesh(const DeviceContext& context, const vk::ArrayProxyNoTemporaries<Vertex>& vertices, const vk::ArrayProxyNoTemporaries<uint32_t>& indices);
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
	Buffer vertex_buffer;
	Buffer index_buffer;
	vk::AccelerationStructureKHR blas;
	Buffer as_buffer;
};;