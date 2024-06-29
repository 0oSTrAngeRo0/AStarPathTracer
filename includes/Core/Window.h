#pragma once

#include <vulkan/vulkan.hpp>
#include <expected>

class Window {
public:
	virtual std::vector<const char*> GetVulkanExtensions() const = 0;
	virtual std::expected<vk::SurfaceKHR, vk::Result> CreateWindowSurface(const vk::Instance instance, const vk::AllocationCallbacks* allocator) const = 0;
	virtual vk::Extent2D GetActualExtent() const = 0;
};