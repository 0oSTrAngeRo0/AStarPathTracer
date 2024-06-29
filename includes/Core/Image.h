#pragma once

class DeviceContext;

class Image {
private:
	vk::Image image;
	vma::Allocation allocation;
public:
	Image(
		const DeviceContext& context,
		const vk::ImageCreateInfo& create_info,
		const vma::AllocationCreateInfo allocation_info = vma::AllocationCreateInfo({}, vma::MemoryUsage::eAuto)
	);
	void Destroy(const DeviceContext& context);
	inline operator vk::Image() const { return image; }
};