#pragma once

import vulkan_hpp;
import Core;

class Surface {
public:
	Surface(const DeviceContext& context, vk::SurfaceKHR surface) : surface(surface) { ReacquireProperties(context); }
	void ReacquireProperties(const DeviceContext& context);
	void Destroy(const DeviceContext& context);
	inline operator vk::SurfaceKHR() const { return surface; }
	inline vk::SurfaceCapabilitiesKHR GetSurfaceCapabilities() const { return capabilities; }
	inline vk::SurfaceFormatKHR GetSurfaceFormat() const { return format; }
	inline vk::PresentModeKHR GetSurfacePresentModes() const { return present_mode; }
private:
	vk::SurfaceKHR surface;
	vk::SurfaceCapabilitiesKHR capabilities;
	vk::SurfaceFormatKHR format;
	vk::PresentModeKHR present_mode;
};