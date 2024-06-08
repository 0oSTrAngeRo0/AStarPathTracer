#include "buffer-facroty.h"

uint32_t FindMemoryType(vk::PhysicalDevice physical, uint32_t filter) {
	vk::PhysicalDeviceMemoryProperties memory_properties = physical.getMemoryProperties();

	bool has_type = false;
	uint32_t type;
	vk::MemoryPropertyFlags properties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
	for (uint32_t i = 0; i < memory_properties.memoryTypeCount; i++) {
		if ((filter & (1 << i)) && (memory_properties.memoryTypes[i].propertyFlags & properties) == properties) {
			type = i;
			has_type = true;
			break;
		}
	}
	if (!has_type) {
		throw std::runtime_error("Failed to find suitable memory type!");
	}
	return type;
}

Buffer CreateBuffer(vk::PhysicalDevice physical, vk::Device device, vk::DeviceSize size, vk::BufferUsageFlags usages, vk::MemoryPropertyFlags properties) {
	vk::BufferCreateInfo create_info({}, size, usages, vk::SharingMode::eExclusive);
	vk::Buffer buffer = device.createBuffer(create_info);
	auto memory_requirements = device.getBufferMemoryRequirements(buffer);
	vk::MemoryAllocateInfo allocate_info(memory_requirements.size, FindMemoryType(physical, memory_requirements.memoryTypeBits));
	vk::DeviceMemory memory = device.allocateMemory(allocate_info);
	device.bindBufferMemory(buffer, memory, 0);
	return Buffer(buffer, memory);
}

void MapMemory(const vk::Device& device, const Buffer& buffer, uint64_t offset) {}