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
	temp_cmd_pool = TemporaryCommandBufferPool(device, physical_device.graphics_queue_index, graphics_queue);
	CreateGraphicsPipeline();
	CreateFrameBuffers();
	CreateCommandPool();
	CreateCommandBuffer();
	CreateSyncObjects();
}

void VulkanApp::OnExecute(MainWindow::OnDestroyed) {
	device.waitIdle();
	device.destroySemaphore(image_available_semaphore);
	device.destroySemaphore(render_finished_semaphore);
	device.destroyFence(in_flight_fence);
	device.destroyCommandPool(command_pool);
	temp_cmd_pool.Destroy(device);
	mesh.Destroy(device);
	for (vk::Framebuffer framebuffer : swapchain_framebuffers) {
		device.destroyFramebuffer(framebuffer);
	}
	device.destroyPipeline(graphics_pipeline);
	device.destroyPipelineLayout(pipeline_layout);
	device.destroyRenderPass(render_pass);
	for (vk::ImageView image_view : swapchain_info.image_views) {
		device.destroyImageView(image_view);
	}
	device.destroySwapchainKHR(swapchain_info.swapchain);
	device.destroy();
	if (config.enable_debug) {
		instance.destroyDebugUtilsMessengerEXT(debugger);
	}
	instance.destroySurfaceKHR(surface);
	instance.destroy();
}
