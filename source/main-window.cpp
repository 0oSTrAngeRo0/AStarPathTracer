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

std::vector<const char *> MainWindow::GetVulkanExtensions() {
    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
    return extensions;
}

void MainWindow::CreateWindowSurface(VkInstance instance, VkAllocationCallbacks *allocator, VkSurfaceKHR *surface) {
    VkResult result = glfwCreateWindowSurface(instance, window, allocator, surface);
    if (result == VK_SUCCESS) return;
    throw std::runtime_error("Failed to create window surface");
}

VkExtent2D MainWindow::GetActualExtent() {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    VkExtent2D extent{};
    extent.width = width;
    extent.height = height;
    return extent;
}
