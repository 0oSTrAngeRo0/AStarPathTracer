#include <stdexcept>
#include "vulkan-utils.h"
#include "buffer.h"
//#include "temporary-command-buffer.h"

Buffer::Buffer(vk::Buffer& handle, vk::DeviceMemory& memory)
{
	this->handle = handle;
	this->memory = memory;
}

void Buffer::Destroy(const vk::Device& device)
{
	device.destroyBuffer(handle);
	device.freeMemory(memory);
}
