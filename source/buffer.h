#ifndef PATHTRACER_SOURCE_BUFFER_H_
#define PATHTRACER_SOURCE_BUFFER_H_

#include "vulkan/vulkan.hpp"

struct Buffer {
	VkBuffer handle;
	VkDeviceAddress device_address;
	VkDeviceMemory memory;

public:
	void Create(const VkPhysicalDevice& physical_device,
		const VkDevice& device,
		const VkDeviceSize& size,
		VkBufferUsageFlags usage);
	void SetData(VkDevice const& device, void* data, const VkDeviceSize& size, VkMemoryMapFlags flags);
	void Destroy(const VkDevice& device) const;
};

#endif //PATHTRACER_SOURCE_BUFFER_H_
