#ifndef PATHTRACER_VULKAN_APP_H
#define PATHTRACER_VULKAN_APP_H

#include "event-registry.h"
#include "main-window.h"
#include "volk.h"
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
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> present_modes;
    };

    struct PhysicalDeviceInfo {
        VkPhysicalDevice device;
        uint32_t graphics_queue_index;
        uint32_t present_queue_index;
		SwapchainSupportInfo swapchain_info;
    };

    struct SwapchainRuntimeInfo {
        VkSwapchainKHR swapchain;
        std::vector<VkImage> images;
        VkFormat format;
        VkExtent2D extent;
        std::vector<VkImageView> image_views;
    };

private:
    VkInstance instance;
    AppConfig config;
    VkDebugUtilsMessengerEXT debugger;
    VkDevice device;
    VkQueue graphics_queue;
    VkQueue present_queue;
    VkSurfaceKHR surface;
    PhysicalDeviceInfo physical_device;
    VkExtent2D actual_extent;
    SwapchainRuntimeInfo swapchain_info;
    VkPipelineLayout pipeline_layout;
    VkRenderPass render_pass;
    VkPipeline graphics_pipeline;
    std::vector<VkFramebuffer> swapchain_framebuffers;
    VkCommandPool command_pool;
    VkCommandBuffer command_buffer;
    VkSemaphore image_available_semaphore;
    VkSemaphore render_finished_semaphore;
    VkFence in_flight_fence;

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
    void RecordCommandBuffer(VkCommandBuffer buffer, uint32_t image_index);
    void CreateSyncObjects();
	void CreateDebugger(bool enable_debugger);

private:

    static std::vector<PhysicalDeviceInfo> GetPhysicalDevices(const VkInstance &instance, const VkSurfaceKHR &surface);

public:
    void OnExecute(MainWindow::OnDestroyed) override;
    void OnExecute(MainWindow::OnDrawFrame) override;
};


#endif //PATHTRACER_VULKAN_APP_H
