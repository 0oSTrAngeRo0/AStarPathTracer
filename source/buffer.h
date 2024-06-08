#ifndef PATHTRACER_SOURCE_BUFFER_H_
#define PATHTRACER_SOURCE_BUFFER_H_

#include <vulkan/vulkan.hpp>

class Buffer {
public:
	vk::Buffer handle;
	vk::DeviceMemory memory;
	Buffer() {}
	Buffer(vk::Buffer& handle, vk::DeviceMemory& memory);
	void Destroy(const vk::Device& device);
};

#endif //PATHTRACER_SOURCE_BUFFER_H_
