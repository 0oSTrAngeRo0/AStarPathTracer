#ifndef PATHTRACER_SOURCE_BUFFER_FACTORY_H
#define PATHTRACER_SOURCE_BUFFER_FACTORY_H

#include <vulkan/vulkan.hpp>
#include "buffer.h"
#include "temporary-command-buffer.h"

Buffer CreateBuffer(vk::PhysicalDevice physical, vk::Device device, vk::DeviceSize size, vk::BufferUsageFlags usages, vk::MemoryPropertyFlags properties);

template <typename T>
void MapMemory(const vk::Device& device, const Buffer& buffer, uint64_t& offset, const vk::ArrayProxy<T>& data) {
	vk::DeviceSize size = sizeof(T) * data.size();
	void* target = device.mapMemory(buffer.memory, offset, size);
	memcpy(target, data.data(), size);
	device.unmapMemory(buffer.memory);
	offset += size;
}

template <typename... TArrays>
Buffer CreateGraphicsBuffer(const vk::PhysicalDevice& physical, const vk::Device& device, TemporaryCommandBufferPool& pool,
	vk::BufferUsageFlags usage, vk::ArrayProxy<TArrays>&... arrays) {

	vk::DeviceSize size = ((sizeof(TArrays) * arrays.size()) + ...);
	Buffer staging_buffer = CreateBuffer(physical, device, size,
		vk::BufferUsageFlagBits::eTransferSrc,
		vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

	uint64_t current = 0;
	(MapMemory(device, staging_buffer, current, arrays), ...);

	Buffer buffer = CreateBuffer(physical, device, size,
		vk::BufferUsageFlagBits::eTransferDst | usage,
		vk::MemoryPropertyFlagBits::eDeviceLocal);

	auto cmd = pool.Get();
	std::vector<vk::BufferCopy> copy_region = {
		vk::BufferCopy(0, 0, size)
	};
	cmd.Buffer().copyBuffer(staging_buffer.handle, buffer.handle, copy_region);
	cmd.Submit();
	pool.Release(cmd);
	device.destroyBuffer(staging_buffer.handle);
	device.freeMemory(staging_buffer.memory);
	return buffer;
}

#endif // !PATHTRACER_SOURCE_BUFFER_FACTORY_H
