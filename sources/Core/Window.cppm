export module Core:Window;

import vulkan_hpp;
import std;

export class VulkanWindow {
public:
	virtual std::vector<std::string> GetVulkanExtensions() const = 0;
	virtual std::expected<vk::SurfaceKHR, vk::Result> CreateWindowSurface(const vk::Instance instance, const vk::AllocationCallbacks* allocator) const = 0;
	virtual vk::Extent2D GetActualExtent() const = 0;
	virtual bool IsResized() const = 0;
	virtual bool IsActive() const = 0;
	virtual ~VulkanWindow() = default;
};