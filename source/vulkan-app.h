#ifndef PATHTRACER_VULKAN_APP_H
#define PATHTRACER_VULKAN_APP_H

#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1

#include "event-registry.h"
#include "main-window.h"
#include "vulkan/vulkan.hpp"
#include <vector>
#include "temporary-command-buffer.h"
#include "mesh.h"

class VulkanApp : public IEventHandler<MainWindow::OnDestroyed>, public IEventHandler<MainWindow::OnDrawFrame> {
public:
	struct OnDebug {
		VkDebugUtilsMessageSeverityFlagBitsEXT message_severity;
		VkDebugUtilsMessageTypeFlagsEXT message_type;
		const VkDebugUtilsMessengerCallbackDataEXT* p_callback_data;
		void* p_user_data;
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
	TemporaryCommandBufferPool temp_cmd_pool;
	Mesh mesh;

public:
	VulkanApp(const AppConfig& config, MainWindow* main_window) {
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
		mesh.vertices = {
			{-0.5f, -0.5f, 0.0f},
			{0.5f, -0.5f, 0.0f},
			{0.5f, 0.5f, 0.0f},
			{-0.5f, 0.5f, 0.0f}
		};
		mesh.indices = {
			0, 1, 2, 2, 3, 0
		};
		mesh.UploadData(physical_device.device, device, temp_cmd_pool);
		CreateGraphicsPipeline();
		CreateFrameBuffers();
		CreateCommandPool();
		CreateCommandBuffer();
		CreateSyncObjects();
	}

private:
	void Create(const AppConfig& config, MainWindow* main_window);

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

	static std::vector<PhysicalDeviceInfo> GetPhysicalDevices(const vk::Instance& instance, const vk::SurfaceKHR& surface);

public:
	void OnExecute(MainWindow::OnDestroyed) override;
	void OnExecute(MainWindow::OnDrawFrame) override;
};


#endif //PATHTRACER_VULKAN_APP_H
