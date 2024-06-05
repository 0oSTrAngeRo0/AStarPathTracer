#include "vulkan-app.h"
#include <stdexcept>
#include "model-loader.h"
#include "buffer.h"

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

void VulkanApp::Create(const AppConfig& config, MainWindow* main_window) {
	this->config = config;

	CreateInstance();

	CreateDebugger(config.enable_debug);

	VkSurfaceKHR raw_surface;
	main_window->CreateWindowSurface(instance, nullptr, &raw_surface);
	surface = vk::SurfaceKHR(raw_surface);
	actual_extent = main_window->GetActualExtent();
	std::vector<PhysicalDeviceInfo> physical_devices = GetPhysicalDevices(instance, surface);
	if (physical_devices.empty()) {
		throw std::runtime_error("No suitable devices!");
	}
	physical_device = physical_devices[0];
	CreateDevice();

	CreateSwapchain();
	CreateGraphicsPipeline();
	CreateFrameBuffers();
	CreateCommandPool();
	CreateCommandBuffer();
	CreateSyncObjects();
}

void VulkanApp::OnExecute(MainWindow::OnDestroyed) {
	vkDeviceWaitIdle(device);
	vkDestroySemaphore(device, image_available_semaphore, nullptr);
	vkDestroySemaphore(device, render_finished_semaphore, nullptr);
	vkDestroyFence(device, in_flight_fence, nullptr);
	vkDestroyCommandPool(device, command_pool, nullptr);
	for (VkFramebuffer framebuffer : swapchain_framebuffers) {
		vkDestroyFramebuffer(device, framebuffer, nullptr);
	}
	vkDestroyPipeline(device, graphics_pipeline, nullptr);
	vkDestroyPipelineLayout(device, pipeline_layout, nullptr);
	vkDestroyRenderPass(device, render_pass, nullptr);
	for (VkImageView image_view : swapchain_info.image_views) {
		vkDestroyImageView(device, image_view, nullptr);
	}
	vkDestroySwapchainKHR(device, swapchain_info.swapchain, nullptr);
	vkDestroyDevice(device, nullptr);
	if (config.enable_debug) {
		instance.destroyDebugUtilsMessengerEXT(debugger);
	}
	vkDestroySurfaceKHR(instance, surface, nullptr);
	vkDestroyInstance(instance, nullptr);
	printf("vulkan app destroyed.");
}
