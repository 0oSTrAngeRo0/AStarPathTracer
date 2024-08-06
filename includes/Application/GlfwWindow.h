#pragma once

#include "config.h"
#include "vulkan/vulkan.hpp"
#include "GLFW/glfw3.h"
#include <vector>
#include "Core/Window.h"
#include <functional>

class GlfwWindow : public Window {
private:
	AppConfig config;
	GLFWwindow* window;
	static uint32_t window_count;
public:
	GlfwWindow(const AppConfig& config);
	~GlfwWindow();
	bool ShouldClose() const;
	void Update() const;
	inline operator GLFWwindow* () const { return window; }
	inline GLFWwindow* GetRawPtr() const { return window; }
	std::vector<const char*> GetVulkanExtensions() const override;
	std::expected<vk::SurfaceKHR, vk::Result> CreateWindowSurface(const vk::Instance instance, const vk::AllocationCallbacks* allocator) const override;
	vk::Extent2D GetActualExtent() const override;
};
