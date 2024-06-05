#include <stdexcept>
#include <optional>
#include "vulkan-app.h"
#include "vulkan-utils.h"
#include "algorithm-util.h"

static bool GetSuitableQueueFamilies(const vk::PhysicalDevice& device, VulkanApp::PhysicalDeviceInfo* device_info, const vk::SurfaceKHR& surface) {
	auto properties = device.getQueueFamilyProperties();
	bool contains_graphics = false;
	bool contains_present = false;
	for (int i = 0, end = properties.size(); i < end; ++i) {
		const vk::QueueFamilyProperties& property = properties[i];
		if (property.queueFlags & vk::QueueFlagBits::eGraphics) {
			device_info->graphics_queue_index = i;
			contains_graphics = true;
		}

		auto is_present_supported = device.getSurfaceSupportKHR(i, surface);
		if (is_present_supported) {
			device_info->present_queue_index = i;
			contains_present = true;
		}
	}
	return contains_graphics && contains_present;
}

std::optional<VulkanApp::SwapchainSupportInfo> GetSwapSupportInfo(const vk::PhysicalDevice device, const vk::SurfaceKHR surface) {
	auto formats = device.getSurfaceFormatsKHR(surface);
	auto present_modes = device.getSurfacePresentModesKHR(surface);

	auto format = Find(formats, vk::SurfaceFormatKHR(vk::Format::eR8G8B8A8Srgb, vk::ColorSpaceKHR::eSrgbNonlinear));
	auto present_mode = Find(present_modes, vk::PresentModeKHR::eMailbox);

	if (!format.has_value() || !present_mode.has_value()) return std::nullopt;

	VulkanApp::SwapchainSupportInfo swapchain_info;
	swapchain_info.capabilities = device.getSurfaceCapabilitiesKHR(surface);
	swapchain_info.formats = format.value();
	swapchain_info.present_modes = present_mode.value();
	return swapchain_info;
}

std::vector<VulkanApp::PhysicalDeviceInfo> VulkanApp::GetPhysicalDevices(const vk::Instance& instance, const vk::SurfaceKHR& surface) {
	auto physical_devices = instance.enumeratePhysicalDevices();

	std::vector<VulkanApp::PhysicalDeviceInfo> candidates;
	for (const vk::PhysicalDevice& device : physical_devices) {
		VulkanApp::PhysicalDeviceInfo info;
		info.device = device;

		// vk::PhysicalDeviceProperties properties = device.getProperties();

		// check devices featured is supported
		auto features = device.getFeatures2<
			vk::PhysicalDeviceFeatures2,
			vk::PhysicalDeviceAccelerationStructureFeaturesKHR,
			vk::PhysicalDeviceRayTracingPipelineFeaturesKHR
		>();
		if (!features.get<vk::PhysicalDeviceAccelerationStructureFeaturesKHR>().accelerationStructure) continue;
		if (!features.get<vk::PhysicalDeviceRayTracingPipelineFeaturesKHR>().rayTracingPipeline) continue;

		// get swapchain info
		auto swapchain_info = GetSwapSupportInfo(device, surface);
		if (!swapchain_info.has_value()) continue;
		info.swapchain_info = swapchain_info.value();

		// check queue families is supported
		bool isSuitable = GetSuitableQueueFamilies(device, &info, surface);
		if (!isSuitable) continue;

		candidates.push_back(info);
	}
	return candidates;
}

void VulkanApp::CreateDevice() {
	VulkanApp::PhysicalDeviceInfo info = physical_device;

	std::vector<vk::DeviceQueueCreateInfo> queue_create_infos;

	std::vector<float> queue_priorities = { 1.0f };
	queue_create_infos.push_back(vk::DeviceQueueCreateInfo({}, info.graphics_queue_index, queue_priorities, {}));
	queue_create_infos.push_back(vk::DeviceQueueCreateInfo({}, info.present_queue_index, queue_priorities, {}));

	std::vector<const char*> extensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME,

		VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME, VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME,

		VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME, VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,
		VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME,

		VK_KHR_SPIRV_1_4_EXTENSION_NAME,

		VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME,
		VK_EXT_ROBUSTNESS_2_EXTENSION_NAME };

	auto features = info.device.getFeatures2<
		vk::PhysicalDeviceFeatures2,
		vk::PhysicalDeviceBufferDeviceAddressFeatures,
		vk::PhysicalDeviceDynamicRenderingFeatures,
		vk::PhysicalDeviceSynchronization2Features,
		vk::PhysicalDeviceDescriptorIndexingFeatures,
		vk::PhysicalDeviceMaintenance4Features,
		vk::PhysicalDeviceAccelerationStructureFeaturesKHR,
		vk::PhysicalDeviceRayTracingPipelineFeaturesKHR,
		vk::PhysicalDeviceRobustness2FeaturesEXT
	>();

	vk::DeviceCreateInfo create_info({}, queue_create_infos, {}, extensions, 0, &features.get());
	device = info.device.createDevice(create_info);

	graphics_queue = device.getQueue(info.graphics_queue_index, 0);
	present_queue = device.getQueue(info.present_queue_index, 0);
}
