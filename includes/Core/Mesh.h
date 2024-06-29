#pragma once

#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1

#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>
#include "Core/Buffer.h"

class DeviceContext;

struct Vertex {
	glm::vec3 position;

	Vertex(glm::vec3 position) : position(position) {}
};

class Mesh {
public:
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	void UploadData(const DeviceContext& context);
	void Destroy(const DeviceContext& context);
	inline vk::DeviceAddress GetVertexAddress() const { return vertex_buffer.GetDeviceAddress(); }
	inline vk::DeviceAddress GetIndexAddress() const { return index_buffer.GetDeviceAddress(); }
	inline vk::DeviceSize GetVertexStride() const { return sizeof(glm::vec3); }
	inline vk::DeviceAddress GetAsAddress() const { return as_buffer.GetDeviceAddress(); }
	inline void SetAsData(vk::AccelerationStructureKHR blas, Buffer buffer) {
		this->as_buffer = buffer;
		this->blas = blas;
	}
private:
	Buffer vertex_buffer;
	Buffer index_buffer;
	vk::AccelerationStructureKHR blas;
	Buffer as_buffer;
};;