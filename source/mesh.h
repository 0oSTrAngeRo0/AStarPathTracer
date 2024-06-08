#ifndef PATHTRACER_MESH_H_
#define PATHTRACER_MESH_H_

#include <glm/glm.hpp>
#include <vector>
#include <vulkan/vulkan.hpp>
#include "buffer.h"

class TemporaryCommandBufferPool;

class Mesh {
public:
	std::vector<glm::vec3> vertices;
	std::vector<uint32_t> indices;
	void UploadData(const vk::PhysicalDevice& physical, const vk::Device& device, TemporaryCommandBufferPool& pool);
	void Draw(const vk::CommandBuffer& cmd);
	void Destroy(const vk::Device& device);
	vk::PipelineVertexInputStateCreateInfo GetVertexInputStateInfo();
private:
	Buffer buffer;
	uint64_t vertices_offset;
	uint64_t indices_offset;
	uint32_t indices_count;
};

#endif // !PATHTRACER_MESH_H_
