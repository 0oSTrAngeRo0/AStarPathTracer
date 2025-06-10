module Core:Buffer;

import vulkan_hpp;
import Core:DeviceContext;
#include "vk_mem_alloc.hpp"

Buffer::Buffer(const DeviceContext& context, const vk::BufferCreateInfo& create_info, const vma::AllocationCreateInfo& allocation_info) {
	auto allocator = context.GetAllocator();

	auto pair = allocator.createBuffer(create_info, allocation_info);
	buffer = pair.first;
	allocation = pair.second;

	size = create_info.size;
	if (create_info.usage & vk::BufferUsageFlagBits::eShaderDeviceAddress) {
		address = context.GetDevice().getBufferAddress(buffer);
	}
}

void Buffer::Destroy(const DeviceContext& context) {
	if (!buffer) return;
	context.GetAllocator().destroyBuffer(buffer, allocation);
	buffer = nullptr;
	allocation = nullptr;
	size = 0;
	address = 0;
}

void Buffer::SetName(const DeviceContext& context, const std::string& name) {
	if (!buffer) return;
	context.GetDevice().setDebugUtilsObjectNameEXT(vk::DebugUtilsObjectNameInfoEXT(vk::ObjectType::eBuffer, (uint64_t)(VkBuffer)buffer, name.c_str()));
	context.GetAllocator().setAllocationName(allocation, name.c_str());
}

const std::string Buffer::GetName(const DeviceContext& context) const {
	const char* chars = context.GetAllocator().getAllocationInfo(allocation).pName;
	return std::string(chars);
}

void Buffer::SetData(const DeviceContext& context, void* data, vk::DeviceSize size){
	context.GetAllocator().copyMemoryToAllocation(data, allocation, 0, size);
}

Buffer Buffer::CreateWithData(
	const DeviceContext& context,
	const vk::BufferCreateInfo& create_info,
	vma::AllocationCreateInfo& allocation_info,
	const void* data
) {
	allocation_info.flags |= vma::AllocationCreateFlagBits::eHostAccessSequentialWrite;
	Buffer buffer(context, create_info, allocation_info);
	context.GetAllocator().copyMemoryToAllocation(data, buffer.allocation, 0, create_info.size);
	return buffer;
}

Buffer Buffer::CreateWithStaging(
	const DeviceContext& context,
	vk::BufferCreateInfo& create_info,
	const vma::AllocationCreateInfo& allocation_info,
	const void* data
) {
	vk::BufferCreateInfo staging_ci({}, create_info.size, vk::BufferUsageFlagBits::eTransferSrc);
	vma::AllocationCreateInfo staging_allocation(vma::AllocationCreateFlagBits::eHostAccessSequentialWrite, vma::MemoryUsage::eAuto);
	Buffer staging_buffer(context, staging_ci, staging_allocation);
	context.GetAllocator().copyMemoryToAllocation(data, staging_buffer.allocation, 0, create_info.size);

	create_info.usage |= vk::BufferUsageFlagBits::eTransferDst;
	Buffer buffer(context, create_info, allocation_info);

	vk::CommandBuffer cmd = context.GetTempCmd();
	cmd.copyBuffer(staging_buffer.buffer, buffer.buffer, vk::BufferCopy(0, 0, create_info.size));
	context.ReleaseTempCmd(cmd);

	staging_buffer.Destroy(context);
	return buffer;
}
