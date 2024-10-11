#include "Core/Surface.h"
#include "Core/DeviceContext.h"

void Surface::ReacquireProperties(const DeviceContext& context) {
	auto gpu = context.GetGpu();
	if (!gpu.getSurfaceSupportKHR(context.GetPresentQueueIndex(), surface))
		throw std::runtime_error("Current present queue [" + std::to_string(context.GetPresentQueueIndex()) + "] is not support current surface");

	auto formats = gpu.getSurfaceFormatsKHR(surface);
	auto present_modes = gpu.getSurfacePresentModesKHR(surface);

	auto format = std::find(formats.begin(), formats.end(), vk::SurfaceFormatKHR(vk::Format::eR8G8B8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear));
	auto present_mode = std::find(present_modes.begin(), present_modes.end(), vk::PresentModeKHR::eMailbox);

	if (format == formats.end() || present_mode == present_modes.end()) {
		throw std::runtime_error("Failed to find surface format or surface present mode");
	}

	capabilities = gpu.getSurfaceCapabilitiesKHR(surface);
	this->format = *format;
	this->present_mode = *present_mode;
}

void Surface::Destroy(const DeviceContext& context) {
	if (surface) {
		context.GetInstance().destroySurfaceKHR(surface);
	}
}
