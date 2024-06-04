#include <stdexcept>
#include "buffer.h"
#include "vulkan-utils.h"

void Buffer::Destroy(VkDevice const& device) const {
	vkDestroyBuffer(device, handle, nullptr);
}

void Buffer::Create(const VkPhysicalDevice& physical_device,
	const VkDevice& device,
	const VkDeviceSize& size,
	VkBufferUsageFlags usage) {

	// create buffer
	VkBufferCreateInfo create_info;
	create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	create_info.pNext = nullptr;
	create_info.usage = usage | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
	create_info.size = size;
	create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	create_info.flags = 0;
	VK_CHECK(vkCreateBuffer(device, &create_info, nullptr, &handle));

	VkMemoryRequirements memory_requirements;
	vkGetBufferMemoryRequirements(device, handle, &memory_requirements);

	VkPhysicalDeviceMemoryProperties memory_properties;
	vkGetPhysicalDeviceMemoryProperties(physical_device, &memory_properties);

	bool has_type = false;
	uint32_t type;
	uint32_t filter = memory_requirements.memoryTypeBits;
	VkMemoryPropertyFlags properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
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

	VkMemoryAllocateFlagsInfo flags_info;
	flags_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO;
	flags_info.flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT;
	flags_info.pNext = nullptr;

	VkMemoryAllocateInfo allocate_info;
	allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocate_info.allocationSize = memory_requirements.size;
	allocate_info.memoryTypeIndex = type;
	allocate_info.pNext = &flags_info;
	VK_CHECK(vkAllocateMemory(device, &allocate_info, nullptr, &memory));

	VK_CHECK(vkBindBufferMemory(device, handle, memory, 0));

	VkBufferDeviceAddressInfo buffer_address_info{};
	buffer_address_info.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
	buffer_address_info.buffer = handle;
	device_address = vkGetBufferDeviceAddress(device, &buffer_address_info);
}
void Buffer::SetData(VkDevice const& device, void* data, const VkDeviceSize& size, VkMemoryMapFlags flags) {
	void* mapped_data;
	vkMapMemory(device, memory, 0, size, flags, &data);
	memcpy(mapped_data, data, size);
	vkUnmapMemory(device, memory);
}
