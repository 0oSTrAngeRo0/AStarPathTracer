#include "vulkan-app.h"
#include <stdexcept>
#include "model-loader.h"
#include "buffer.h"

void VulkanApp::Create(const AppConfig& config, MainWindow* main_window) {
	this->config = config;

	VkResult initialize_result = volkInitialize();
	if (initialize_result != VK_SUCCESS) {
		throw std::runtime_error("Failed to initialize volk!");
	}

	CreateInstance();

	volkLoadInstance(instance);

	CreateDebugger(config.enable_debug);

	main_window->CreateWindowSurface(instance, nullptr, &surface);
	actual_extent = main_window->GetActualExtent();
	std::vector<PhysicalDeviceInfo> physical_devices = GetPhysicalDevices(instance, surface);
	if (physical_devices.empty()) {
		throw std::runtime_error("No suitable devices!");
	}
	physical_device = physical_devices[0];
	CreateDevice();

	volkLoadDevice(device);

	CreateSwapchain();
	CreateGraphicsPipeline();
	CreateFrameBuffers();
	CreateCommandPool();
	CreateCommandBuffer();
	CreateSyncObjects();

	GpuMesh gpu_mesh;
	asset::Mesh mesh = asset::LoadModelObj((R"(D:\C++\Projects\PathTracer\models\mesh.obj)"), 1.25f);
	{
		VkDeviceSize size = mesh.vertices.size() * sizeof(mesh.vertices[0]);
		VkBufferUsageFlags usage =
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
				| VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR;
		gpu_mesh.vertex_buffer.Create(physical_device.device, device, size, usage);
		gpu_mesh.vertex_count = uint32_t(mesh.vertices.size());
	}
	{
		VkDeviceSize size = mesh.indices.size() * sizeof(mesh.indices[0]);
		VkBufferUsageFlags usage =
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
				| VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR;
		gpu_mesh.index_buffer.Create(physical_device.device, device, size, usage);
		gpu_mesh.index_count = uint32_t(mesh.indices.size());
	}
	acceleration_structure.Create(device, physical_device.device, command_buffer, { gpu_mesh });
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
		vkDestroyDebugUtilsMessengerEXT(instance, debugger, nullptr);
	}
	vkDestroySurfaceKHR(instance, surface, nullptr);
	vkDestroyInstance(instance, nullptr);
	printf("vulkan app destroyed.");
}
