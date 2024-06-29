#pragma once

#include <vulkan/vulkan.hpp>
#include <vk_mem_alloc.hpp>

class DeviceContext;

class Buffer {
private:
	vk::Buffer buffer;
	vma::Allocation allocation;
	vk::DeviceAddress address;
public:
	Buffer() {}

	Buffer(
		const DeviceContext& context,
		const vk::BufferCreateInfo& create_info,
		const vma::AllocationCreateInfo& allocation_info = vma::AllocationCreateInfo({}, vma::MemoryUsage::eAuto)
	);

	template <typename TElement>
	inline void SetData(const DeviceContext& context, vk::ArrayProxyNoTemporaries<TElement> data) {
		SetData(context, data.data(), data.size());
	}

	void Destroy(const DeviceContext& context);
	inline const vk::DeviceAddress GetDeviceAddress() const { return address; }
	inline operator vk::Buffer()const { return buffer; }

private:
	void SetData(const DeviceContext& context, void* data, vk::DeviceSize size);

private:
	static Buffer CreateWithData(
		const DeviceContext& context,
		const vk::BufferCreateInfo& create_info,
		vma::AllocationCreateInfo& allocation_info,
		const void* data
	);

	// Would add vk::BufferUsageFlagBits::eTransferDst to create_info.usages
	static Buffer CreateWithStaging(
		const DeviceContext& context,
		vk::BufferCreateInfo& create_info,
		const vma::AllocationCreateInfo& allocation_info,
		const void* data
	);

public:
	template <typename TElement>
	static Buffer CreateWithData(
		const DeviceContext& context,
		vk::BufferCreateInfo& create_info,
		vma::AllocationCreateInfo& allocation_info,
		const vk::ArrayProxyNoTemporaries<const TElement>& data
	) {
		create_info.size = data.size() * sizeof(TElement);
		return Buffer::CreateWithData(context, create_info, allocation_info, data.data());
	}

	template <typename TElement>
	inline static Buffer CreateWithData(
		const DeviceContext& context,
		vk::BufferCreateInfo& create_info,
		const vk::ArrayProxyNoTemporaries<const TElement>& data
	) {
		vma::AllocationCreateInfo allocation_info = vma::AllocationCreateInfo({}, vma::MemoryUsage::eAuto);
		return Buffer::CreateWithData<TElement>(context, create_info, allocation_info, data);
	}

	template <typename TElement>
	static Buffer CreateWithStaging(
		const DeviceContext& context,
		vk::BufferCreateInfo& create_info,
		const vk::ArrayProxyNoTemporaries<TElement>& data, 
		const vma::AllocationCreateInfo& allocation_info = vma::AllocationCreateInfo({}, vma::MemoryUsage::eAuto)
	) {
		create_info.size = data.size() * sizeof(TElement);
		return Buffer::CreateWithStaging(context, create_info, allocation_info, data.data());
	}
};