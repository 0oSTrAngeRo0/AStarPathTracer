#include "temporary-command-buffer.h"
#include <stdexcept>

const vk::CommandBuffer& TemporaryCommandBuffer::Buffer()
{
	return buffer;
}

TemporaryCommandBuffer::TemporaryCommandBuffer(const vk::Device& device, const vk::CommandPool& pool, const vk::Queue& queue)
{
	this->device = device;
	this->pool = pool;
	this->queue = queue;
	vk::CommandBufferAllocateInfo allocate_info(pool, vk::CommandBufferLevel::ePrimary, 1);
	buffer = device.allocateCommandBuffers(allocate_info)[0];
	buffer.begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
}

void TemporaryCommandBuffer::Submit()
{
	buffer.end();
	std::vector<vk::CommandBuffer> command_buffers = {
		buffer,
	};
	std::vector<vk::SubmitInfo> submit_infos = {
		vk::SubmitInfo({}, {}, command_buffers)
	};
	queue.submit(submit_infos);
	queue.waitIdle();
	device.freeCommandBuffers(pool, command_buffers);
}

const vk::CommandPool& TemporaryCommandBufferPool::Pool()
{
	return pool;
}

TemporaryCommandBufferPool::TemporaryCommandBufferPool(const vk::Device& device, const uint32_t queue_family_index, const vk::Queue& queue)
{
	vk::CommandPoolCreateInfo create_info(vk::CommandPoolCreateFlagBits::eTransient, queue_family_index);
	this->device = device;
	this->queue = queue;
	pool = device.createCommandPool(create_info);
}

TemporaryCommandBuffer TemporaryCommandBufferPool::Get()
{
	return TemporaryCommandBuffer(device, pool, queue);
}

void TemporaryCommandBufferPool::Release(const TemporaryCommandBuffer& buffer)
{
	
}

void TemporaryCommandBufferPool::Destroy(const vk::Device device)
{
	device.destroy(pool);
}
