#ifndef PATHTRACER_VERTEX_H_
#define PATHTRACER_VERTEX_H_

#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>
#include <array>

struct Vertex {
	glm::vec2 position;
	glm::vec3 color;

	static vk::PipelineVertexInputStateCreateInfo GetGraphicsPipelineVertexInputState();
};

#endif