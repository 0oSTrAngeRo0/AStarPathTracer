#ifndef PATHTRACER_MAIN_WINDOW_H
#define PATHTRACER_MAIN_WINDOW_H

#include "config.h"
#include "volk.h"
//#include "vulkan/vulkan.hpp"
#include "GLFW/glfw3.h"
#include <vector>

class MainWindow {

public:
	struct OnDestroyed {};
	struct OnDrawFrame {};
private:
	GLFWwindow* window;

public:
	void Create(const AppConfig& config);
	void Run();
	std::vector<const char*> GetVulkanExtensions();
	void CreateWindowSurface(VkInstance instance, VkAllocationCallbacks* allocator, VkSurfaceKHR* surface);
	VkExtent2D GetActualExtent();
};

#endif //PATHTRACER_MAIN_WINDOW_H
