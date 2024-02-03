#include <stdexcept>
#include "vulkan-app.h"
#include "vulkan-utils.h"

void GetQueueCreateInfo(VkDeviceQueueCreateInfo* create_info, float* priority, uint32_t index);

VulkanApp::SwapchainSupportInfo GetSwapchainSupportInfo(VkPhysicalDevice device, VkSurfaceKHR surface);

bool GetSuitableQueueFamilies(const VkPhysicalDevice& device,
	VulkanApp::PhysicalDeviceInfo* device_info,
	const VkSurfaceKHR& surface);

bool IsPhysicalDeviceFeaturesSuitable(const VkPhysicalDevice& device) {
	VkPhysicalDeviceFeatures2 features2{};
	features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;

	VkPhysicalDeviceAccelerationStructureFeaturesKHR acceleration_structure_features;
	acceleration_structure_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;

	VkPhysicalDeviceRayTracingPipelineFeaturesKHR ray_tracing_pipeline_features;
	ray_tracing_pipeline_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;

	features2.pNext = &acceleration_structure_features;
	acceleration_structure_features.pNext = &ray_tracing_pipeline_features;
	ray_tracing_pipeline_features.pNext = nullptr;

	vkGetPhysicalDeviceFeatures2(device, &features2);
	if (!acceleration_structure_features.accelerationStructure || !ray_tracing_pipeline_features.rayTracingPipeline) {
		return false;
	}
	return true;
}

std::vector<VulkanApp::PhysicalDeviceInfo> VulkanApp::GetPhysicalDevices(const VkInstance& instance,
	const VkSurfaceKHR& surface) {
	uint32_t devices_count = 0;
	vkEnumeratePhysicalDevices(instance, &devices_count, nullptr);
	if (devices_count == 0) {
		throw std::runtime_error("Error: No gpu devices");
	}
	std::vector<VkPhysicalDevice> devices(devices_count);
	vkEnumeratePhysicalDevices(instance, &devices_count, devices.data());

	std::vector<VulkanApp::PhysicalDeviceInfo> candidates;
	for (auto device : devices) {
		VulkanApp::PhysicalDeviceInfo info{};
		info.device = device;

		VkPhysicalDeviceProperties properties;
		vkGetPhysicalDeviceProperties(device, &properties);

		if (!IsPhysicalDeviceFeaturesSuitable(device)) continue;

		VulkanApp::SwapchainSupportInfo swapchain_info = GetSwapchainSupportInfo(device, surface);
		if (swapchain_info.present_modes.empty() || swapchain_info.formats.empty()) continue;
		info.swapchain_info = swapchain_info;

		bool isSuitable = GetSuitableQueueFamilies(device, &info, surface);
		if (!isSuitable) continue;

		candidates.push_back(info);
	}
	return candidates;
}

void VulkanApp::CreateDevice() {
	VulkanApp::PhysicalDeviceInfo info = physical_device;

	std::vector<VkDeviceQueueCreateInfo> queue_create_infos;

	VkDeviceQueueCreateInfo graphics_create_info{};
	float graphics_priority = 1.0f;
	GetQueueCreateInfo(&graphics_create_info, &graphics_priority, info.graphics_queue_index);
	queue_create_infos.push_back(graphics_create_info);

	VkDeviceQueueCreateInfo present_create_info{};
	float present_priority = 1.0f;
	GetQueueCreateInfo(&present_create_info, &present_priority, info.present_queue_index);
	queue_create_infos.push_back(present_create_info);

	std::vector<const char*> extensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME,

		VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME, VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME,

		VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME, VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,
		VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME,

		VK_KHR_SPIRV_1_4_EXTENSION_NAME,

		VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME,
		VK_EXT_ROBUSTNESS_2_EXTENSION_NAME};

	VkPhysicalDeviceFeatures2 features2;
	features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;

	VkPhysicalDeviceBufferDeviceAddressFeatures buffer_device_address_features;
	buffer_device_address_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES;
	buffer_device_address_features.bufferDeviceAddress = VK_TRUE;

	VkPhysicalDeviceDynamicRenderingFeatures dynamic_rendering_features;
	dynamic_rendering_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES;
	dynamic_rendering_features.dynamicRendering = VK_TRUE;

	VkPhysicalDeviceSynchronization2Features synchronization2_features;
	synchronization2_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES;
	synchronization2_features.synchronization2 = VK_TRUE;

	VkPhysicalDeviceDescriptorIndexingFeatures descriptor_indexing_features;
	descriptor_indexing_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
	descriptor_indexing_features.runtimeDescriptorArray = VK_TRUE;

	VkPhysicalDeviceMaintenance4Features maintenance4_features;
	maintenance4_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_4_FEATURES;
	maintenance4_features.maintenance4 = VK_TRUE;

	VkPhysicalDeviceAccelerationStructureFeaturesKHR acceleration_structure_features;
	acceleration_structure_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;
	acceleration_structure_features.accelerationStructure = VK_TRUE;

	VkPhysicalDeviceRayTracingPipelineFeaturesKHR ray_tracing_pipeline_features;
	ray_tracing_pipeline_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;
	ray_tracing_pipeline_features.rayTracingPipeline = VK_TRUE;

	VkPhysicalDeviceRobustness2FeaturesEXT robustness2_features;
	robustness2_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ROBUSTNESS_2_FEATURES_EXT;
	robustness2_features.nullDescriptor = VK_TRUE;

	features2.pNext = &buffer_device_address_features;
	buffer_device_address_features.pNext = &dynamic_rendering_features;
	dynamic_rendering_features.pNext = &synchronization2_features;
	synchronization2_features.pNext = &descriptor_indexing_features;
	descriptor_indexing_features.pNext = &maintenance4_features;
	maintenance4_features.pNext = &acceleration_structure_features;
	acceleration_structure_features.pNext = &ray_tracing_pipeline_features;
	ray_tracing_pipeline_features.pNext = &robustness2_features;
	robustness2_features.pNext = nullptr;

	vkGetPhysicalDeviceFeatures2(info.device, &features2);

	VkDeviceCreateInfo create_info{};
	create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	create_info.pQueueCreateInfos = queue_create_infos.data();
	create_info.queueCreateInfoCount = queue_create_infos.size();
	create_info.pEnabledFeatures = nullptr;
	create_info.pNext = &features2;
	create_info.ppEnabledExtensionNames = extensions.data();
	create_info.enabledExtensionCount = extensions.size();
	printf("Start create device\n");
	VkResult result = vkCreateDevice(info.device, &create_info, nullptr, &device);
	if (result != VK_SUCCESS) {
		throw std::runtime_error("Failed to create logic device!");
	}

	vkGetDeviceQueue(device, info.graphics_queue_index, 0, &graphics_queue);
	vkGetDeviceQueue(device, info.present_queue_index, 0, &present_queue);
}

void GetQueueCreateInfo(VkDeviceQueueCreateInfo* create_info, float* priority, uint32_t index) {
	create_info->sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	create_info->queueCount = 1;
	create_info->pQueuePriorities = priority;
	create_info->queueFamilyIndex = index;
}

VulkanApp::SwapchainSupportInfo GetSwapchainSupportInfo(VkPhysicalDevice device, VkSurfaceKHR surface) {
	VulkanApp::SwapchainSupportInfo info{};

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &info.capabilities);

	uint32_t format_count = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, nullptr);
	info.formats.resize(format_count);
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, info.formats.data());

	uint32_t present_mode_count = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_mode_count, nullptr);
	info.present_modes.resize(present_mode_count);
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_mode_count, info.present_modes.data());

	return info;
}

bool GetSuitableQueueFamilies(const VkPhysicalDevice& device,
	VulkanApp::PhysicalDeviceInfo* device_info,
	const VkSurfaceKHR& surface) {
	std::vector<uint32_t> queues;
	uint32_t family_count = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &family_count, nullptr);
	if (family_count == 0) return false;
	std::vector<VkQueueFamilyProperties> properties(family_count);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &family_count, properties.data());
	bool contains_graphics = false;
	bool contains_present = false;
	for (int i = 0; i < family_count; ++i) {
		const VkQueueFamilyProperties& property = properties[i];
		if (property.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			device_info->graphics_queue_index = i;
			contains_graphics = true;
		}

		VkBool32 present_support = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &present_support);
		if (present_support) {
			device_info->present_queue_index = i;
			contains_present = true;
		}
	}
	return contains_graphics & contains_present;
}


