#include "vertex.h"

vk::PipelineVertexInputStateCreateInfo Vertex::GetGraphicsPipelineVertexInputState()
{
	std::vector<vk::VertexInputBindingDescription> bindings = {
		vk::VertexInputBindingDescription(0, sizeof(Vertex), vk::VertexInputRate::eVertex),
	};

	std::vector<vk::VertexInputAttributeDescription> attributes = {
		vk::VertexInputAttributeDescription(0, 0, vk::Format::eR32G32Sfloat, offsetof(Vertex, position)),
		vk::VertexInputAttributeDescription(0, 1, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, color))
	};

	return vk::PipelineVertexInputStateCreateInfo({}, bindings, attributes);
}
