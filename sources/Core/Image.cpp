import Core:DeviceContext;
#include "Core/Image.h"

Image::Image(
	const DeviceContext& context,
	const vk::ImageCreateInfo& create_info,
	const vma::AllocationCreateInfo allocation_info
) {
	auto pair = context.GetAllocator().createImage(create_info, allocation_info);
	image = pair.first;
	allocation = pair.second;
}

void Image::Destroy(const DeviceContext& context) {
	context.GetAllocator().destroyImage(image, allocation);
}
