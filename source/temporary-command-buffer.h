#ifndef PATHTRACER_TEMPORARY_COMMAND_BUFFER_H_
#define PATHTRACER_TEMPORARY_COMMAND_BUFFER_H_

#include <vulkan/vulkan.hpp>

class TemporaryCommandBuffer {
public:
	const vk::CommandBuffer& Buffer();
	TemporaryCommandBuffer(const vk::Device& device, const vk::CommandPool& pool, const vk::Queue& queue);
	void Submit();
private:
	vk::CommandBuffer buffer;
	vk::Device device;
	vk::Queue queue;
	vk::CommandPool pool;
};

class TemporaryCommandBufferPool {
public:
	const vk::CommandPool& Pool();
	TemporaryCommandBufferPool() {}
	TemporaryCommandBufferPool(const vk::Device& device, const uint32_t queue_family_index, const vk::Queue& queue);
	TemporaryCommandBuffer Get();
	void Release(const TemporaryCommandBuffer& buffer);
	void Destroy(const vk::Device device);
private:
	vk::CommandPool pool;
	vk::Queue queue;
	vk::Device device;
};

#endif