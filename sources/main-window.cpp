#define GLFW_INCLUDE_VULKAN

#include <stdexcept>
#include "main-window.h"
#include "GLFW/glfw3.h"
#include "event-registry.h"

void MainWindow::Create(const AppConfig &config) {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    this->window = glfwCreateWindow(config.window_width, config.window_height, config.window_title, nullptr, nullptr);
}

void MainWindow::Run() {
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        EventRegistry<OnDrawFrame>::Broadcast(OnDrawFrame());
    }
    EventRegistry<OnDestroyed>::Broadcast(OnDestroyed());
    glfwDestroyWindow(window);
    glfwTerminate();
}

std::vector<const char *> MainWindow::GetVulkanExtensions() const {
    uint32_t count = 0;
    const char **extensions = glfwGetRequiredInstanceExtensions(&count);
    return std::vector<const char*>(extensions, extensions + count);
}

std::expected<vk::SurfaceKHR, vk::Result> MainWindow::CreateWindowSurface(const vk::Instance instance, const vk::AllocationCallbacks* allocator) const {
    VkSurfaceKHR surface;
    auto raw_allocator = reinterpret_cast<const VkAllocationCallbacks*>(allocator);
    VkResult result = glfwCreateWindowSurface(instance, window, raw_allocator, &surface);
    if (result == VK_SUCCESS) return vk::SurfaceKHR(surface);
    else return std::unexpected(vk::Result(result));
}

vk::Extent2D MainWindow::GetActualExtent() const {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    return vk::Extent2D(width, height);
}
