#ifndef PATHTRACER_MAIN_WINDOW_H
#define PATHTRACER_MAIN_WINDOW_H

#include "config.h"
#include "vulkan/vulkan.hpp"
#include "GLFW/glfw3.h"
#include <vector>
#include "Core/Window.h"

class MainWindow : public Window {

public:
	struct OnDestroyed {};
	struct OnDrawFrame {};
private:
	GLFWwindow* window;

public:
	void Create(const AppConfig& config);
	void Run();
	std::vector<const char*> GetVulkanExtensions() const override;
	std::expected<vk::SurfaceKHR, vk::Result> CreateWindowSurface(const vk::Instance instance, const vk::AllocationCallbacks* allocator) const override;
	vk::Extent2D GetActualExtent() const override;
};

#endif //PATHTRACER_MAIN_WINDOW_H
