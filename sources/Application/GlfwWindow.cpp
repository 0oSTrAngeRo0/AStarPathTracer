#define GLFW_INCLUDE_VULKAN

#include <stdexcept>
#include "Application/GlfwWindow.h"
#include "GLFW/glfw3.h"
#include "event-registry.h"

uint32_t GlfwWindow::window_count = 0;

GlfwWindow::GlfwWindow(const AppConfig& config) : config(config) {
	if (window_count == 0) {
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwSetErrorCallback([](int error_code, const char* message) {
			std::printf("Window Error: [%d], [%s]\n", error_code, message);
		});
		std::printf("Glfw Initialized\n");
	}
	window_count++;
	this->window = glfwCreateWindow(config.window_width, config.window_height, config.window_title, nullptr, nullptr);
	std::printf("Glfw Window [%s] Created\n", config.window_title);
}

GlfwWindow::~GlfwWindow() {
	glfwDestroyWindow(window);
	std::printf("Glfw Window [%s] Destroyed\n", config.window_title);
	window_count--;
	if (window_count == 0) {
		glfwTerminate();
		std::printf("Glfw Finalized\n");
	}
}

bool GlfwWindow::ShouldClose() const {
	return glfwWindowShouldClose(window);
}

void GlfwWindow::Update() const {
	glfwPollEvents();
}

std::vector<const char*> GlfwWindow::GetVulkanExtensions() const {
	uint32_t count = 0;
	const char** extensions = glfwGetRequiredInstanceExtensions(&count);
	return std::vector<const char*>(extensions, extensions + count);
}

std::expected<vk::SurfaceKHR, vk::Result> GlfwWindow::CreateWindowSurface(const vk::Instance instance, const vk::AllocationCallbacks* allocator) const {
	VkSurfaceKHR surface;
	auto raw_allocator = reinterpret_cast<const VkAllocationCallbacks*>(allocator);
	VkResult result = glfwCreateWindowSurface(instance, window, raw_allocator, &surface);
	if (result == VK_SUCCESS) return vk::SurfaceKHR(surface);
	else return std::unexpected(vk::Result(result));
}

vk::Extent2D GlfwWindow::GetActualExtent() const {
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	return vk::Extent2D(width, height);
}
