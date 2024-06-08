#include "buffer-facroty.h"
#include "mesh.h"

void Mesh::UploadData(const vk::PhysicalDevice& physical, const vk::Device& device, TemporaryCommandBufferPool& pool)
{
	vk::BufferUsageFlags usage = vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eIndexBuffer;
	vk::ArrayProxy<glm::vec3> verteicesProxy(vertices);
	vk::ArrayProxy<uint32_t> indicesProxy(indices);
	buffer = CreateGraphicsBuffer(physical, device, pool, usage, verteicesProxy, indicesProxy);
	vertices_offset = 0;
	indices_offset = sizeof(glm::vec3) * vertices.size();
	indices_count = indices.size();
}

void Mesh::Draw(const vk::CommandBuffer& cmd)
{
	std::vector<vk::Buffer> vertex_buffers = {
		buffer.handle
	};
	std::vector<vk::DeviceSize> vertex_offsets = {
		vertices_offset
	};
	cmd.bindVertexBuffers(0, vertex_buffers, vertices_offset);
	cmd.bindIndexBuffer(buffer.handle, indices_offset, vk::IndexType::eUint32);
	cmd.drawIndexed(indices_count, 1, 0, 0, 0);
}

void Mesh::Destroy(const vk::Device& device)
{
	buffer.Destroy(device);
}

vk::PipelineVertexInputStateCreateInfo Mesh::GetVertexInputStateInfo()
{
	static std::vector<vk::VertexInputBindingDescription> bindings = {
		vk::VertexInputBindingDescription(0, sizeof(glm::vec3), vk::VertexInputRate::eVertex)
	};

	static std::vector<vk::VertexInputAttributeDescription> attributes = {
		vk::VertexInputAttributeDescription(0, 0, vk::Format::eR32G32B32Sfloat, 0)
	};
	static vk::PipelineVertexInputStateCreateInfo vertex_input_state({}, bindings, attributes);
	return vertex_input_state;
}
