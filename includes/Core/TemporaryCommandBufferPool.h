#pragma once

#include <vulkan/vulkan.hpp>

class TemporaryCommandBufferPool {
public:
	TemporaryCommandBufferPool(const vk::Device device, const uint32_t queue_family_index);
	const vk::CommandBuffer Get(const vk::Device device) const;
	void Release(const vk::CommandBuffer buffer, const vk::Device device, const vk::Queue queue) const;
	void Destroy(const vk::Device device);
private:
	vk::CommandPool pool;
};
