#ifndef PATHTRACER_VULKAN_APP_H
#define PATHTRACER_VULKAN_APP_H

#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1

#include "event-registry.h"
#include "main-window.h"
#include "vulkan/vulkan.hpp"
#include <vector>

class VulkanApp : public IEventHandler<MainWindow::OnDestroyed>, public IEventHandler<MainWindow::OnDrawFrame> {
public:
    struct OnDebug {
        VkDebugUtilsMessageSeverityFlagBitsEXT message_severity;
        VkDebugUtilsMessageTypeFlagsEXT message_type;
        const VkDebugUtilsMessengerCallbackDataEXT *p_callback_data;
        void *p_user_data;
    };

    struct SwapchainSupportInfo {
        vk::SurfaceCapabilitiesKHR capabilities;
        vk::SurfaceFormatKHR formats;
        vk::PresentModeKHR present_modes;
    };

    struct PhysicalDeviceInfo {
        vk::PhysicalDevice device;
        uint32_t graphics_queue_index;
        uint32_t present_queue_index;
		SwapchainSupportInfo swapchain_info;
    };

    struct SwapchainRuntimeInfo {
        vk::SwapchainKHR swapchain;
        std::vector<vk::Image> images;
        vk::Format format;
        vk::Extent2D extent;
        std::vector<vk::ImageView> image_views;
    };

private:
    vk::Instance instance;
    AppConfig config;
    vk::DebugUtilsMessengerEXT debugger;
    vk::Device device;
    vk::Queue graphics_queue;
    vk::Queue present_queue;
    vk::SurfaceKHR surface;
    PhysicalDeviceInfo physical_device;
    vk::Extent2D actual_extent;
    SwapchainRuntimeInfo swapchain_info;
    vk::PipelineLayout pipeline_layout;
    vk::RenderPass render_pass;
    vk::Pipeline graphics_pipeline;
    std::vector<vk::Framebuffer> swapchain_framebuffers;
    vk::CommandPool command_pool;
    vk::CommandBuffer command_buffer;
    vk::Semaphore image_available_semaphore;
    vk::Semaphore render_finished_semaphore;
    vk::Fence in_flight_fence;

public:
    void Create(const AppConfig &config, MainWindow *main_window);

private:

    void CreateDevice();
    void CreateInstance();
    void CreateSwapchain();
    void CreateGraphicsPipeline();
    void CreateFrameBuffers();
    void CreateCommandPool();
    void CreateCommandBuffer();
    void RecordCommandBuffer(vk::CommandBuffer buffer, uint32_t image_index);
    void CreateSyncObjects();
	void CreateDebugger(bool enable_debugger);

private:

    static std::vector<PhysicalDeviceInfo> GetPhysicalDevices(const vk::Instance &instance, const vk::SurfaceKHR &surface);

public:
    void OnExecute(MainWindow::OnDestroyed) override;
    void OnExecute(MainWindow::OnDrawFrame) override;
};


#endif //PATHTRACER_VULKAN_APP_H
