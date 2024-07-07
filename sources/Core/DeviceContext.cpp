#include "Core/DeviceContext.h"

#define APP_NAME "AStarPathTracer"
#define ENGINE_NAME "AStarPathTracerEngine"

#if ENABLE_DEBUG
#include "Core/EngineDebugger.h"
#endif // ENABLE_DEBUG

#include "Core/Window.h"
#include "Utilities/Algorithm.h"

#pragma region Instance

vk::Instance CreateInstance(const Window& window) {
	vk::ApplicationInfo app_info = vk::ApplicationInfo(APP_NAME, vk::makeVersion(1, 0, 0), ENGINE_NAME, vk::makeVersion(1, 3, 0), vk::ApiVersion13);
	std::vector<const char*> extensions = {
		VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME,

		#if ENABLE_DEBUG
		VK_EXT_DEBUG_UTILS_EXTENSION_NAME
		#endif // ENABLE_DEBUG
	};
	std::vector<const char*> window_extensions = window.GetVulkanExtensions();
	extensions.insert(extensions.end(), window_extensions.begin(), window_extensions.end());

	std::vector<const char*> layers = {

		#if ENABLE_DEBUG
		"VK_LAYER_KHRONOS_validation"
		#endif // ENABLE_DEBUG
	};

	vk::InstanceCreateInfo create_info = vk::InstanceCreateInfo(vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR, &app_info, layers, extensions);
	return vk::createInstance(create_info);
}

#pragma endregion

#pragma region GPU

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
	vk::StructureChain<GPU_FEATURE> features;
	vk::StructureChain<GPU_PROPERTY> properties;
};

bool GetSuitableQueueFamilies(const vk::PhysicalDevice device, PhysicalDeviceInfo* device_info, const vk::SurfaceKHR surface) {
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

std::optional<SwapchainSupportInfo> GetSwapSupportInfo(const vk::PhysicalDevice device, const vk::SurfaceKHR surface) {
	auto formats = device.getSurfaceFormatsKHR(surface);
	auto present_modes = device.getSurfacePresentModesKHR(surface);

	auto format = Find(formats, vk::SurfaceFormatKHR(vk::Format::eR8G8B8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear));
	auto present_mode = Find(present_modes, vk::PresentModeKHR::eMailbox);

	if (!format.has_value() || !present_mode.has_value()) return std::nullopt;

	SwapchainSupportInfo swapchain_info;
	swapchain_info.capabilities = device.getSurfaceCapabilitiesKHR(surface);
	swapchain_info.formats = format.value();
	swapchain_info.present_modes = present_mode.value();
	return swapchain_info;
}

std::vector<PhysicalDeviceInfo> GetPhysicalDevices(const vk::Instance instance, const vk::SurfaceKHR surface) {
	auto physical_devices = instance.enumeratePhysicalDevices();

	std::vector<PhysicalDeviceInfo> candidates;
	for (const vk::PhysicalDevice& device : physical_devices) {
		PhysicalDeviceInfo info;
		info.device = device;

		// vk::PhysicalDeviceProperties properties = device.getProperties();

		// check devices featured is supported
		auto features = device.getFeatures2<GPU_FEATURE>();
		if (!features.get<vk::PhysicalDeviceAccelerationStructureFeaturesKHR>().accelerationStructure) continue;
		if (!features.get<vk::PhysicalDeviceRayTracingPipelineFeaturesKHR>().rayTracingPipeline) continue;
		if (!features.get<vk::PhysicalDevice16BitStorageFeaturesKHR>().storageBuffer16BitAccess) continue;
		info.features = features;

		auto properties = device.getProperties2<GPU_PROPERTY>();
		info.properties = properties;

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

#pragma endregion

#pragma region Device

vk::Device CreateDevice(const vk::PhysicalDevice gpu, const int graphics_queue_index, const int present_queue_index) {
	std::vector<float> queue_priorities = { 1.0f };
	std::vector<vk::DeviceQueueCreateInfo> queue_create_infos = {
		vk::DeviceQueueCreateInfo({}, graphics_queue_index, queue_priorities, {}),
		vk::DeviceQueueCreateInfo({}, present_queue_index, queue_priorities, {})
	};

	std::vector<const char*> extensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,

		VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME,
		VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME,

		VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,
		VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,
		VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME,

		VK_KHR_SPIRV_1_4_EXTENSION_NAME,

		VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME,
		VK_EXT_ROBUSTNESS_2_EXTENSION_NAME
	};

	auto features = gpu.getFeatures2<
		vk::PhysicalDeviceFeatures2,
		vk::PhysicalDeviceBufferDeviceAddressFeatures,
		vk::PhysicalDeviceDynamicRenderingFeatures,
		vk::PhysicalDeviceSynchronization2Features,
		vk::PhysicalDeviceDescriptorIndexingFeatures,
		vk::PhysicalDeviceMaintenance4Features,
		vk::PhysicalDeviceAccelerationStructureFeaturesKHR,
		vk::PhysicalDeviceRayTracingPipelineFeaturesKHR,
		vk::PhysicalDeviceRobustness2FeaturesEXT,
		vk::PhysicalDevice16BitStorageFeaturesKHR
	>();

	return gpu.createDevice(vk::DeviceCreateInfo({}, queue_create_infos, {}, extensions, 0, &features.get()));
}

#pragma endregion

#pragma region Vma

vma::Allocator CreateVmaAllocator(const vk::Instance instance, const vk::PhysicalDevice gpu, const vk::Device device) {
	vma::VulkanFunctions functions = vma::functionsFromDispatcher(VULKAN_HPP_DEFAULT_DISPATCHER);
	return vma::createAllocator(vma::AllocatorCreateInfo(
		vma::AllocatorCreateFlagBits::eExtMemoryBudget | vma::AllocatorCreateFlagBits::eBufferDeviceAddress,
		gpu,
		device,
		{},
		{},
		{},
		{},
		&functions,
		instance,
		vk::ApiVersion13
	));
}

#pragma endregion


DeviceContext::DeviceContext(const Window& window)
{
	VULKAN_HPP_DEFAULT_DISPATCHER.init();
	instance = CreateInstance(window);
	VULKAN_HPP_DEFAULT_DISPATCHER.init(instance);

#if ENABLE_DEBUG
	debugger.reset(new EngineDebugger(instance));
#endif // ENABLE_DEBUG

	auto result = window.CreateWindowSurface(instance, nullptr);
	if (!result.has_value()) throw std::runtime_error("Failed to create window surface. Error: " + vk::to_string(result.error()));
	surface = result.value();
	extent = window.GetActualExtent();

	std::vector<PhysicalDeviceInfo> physical_devices = GetPhysicalDevices(instance, surface);
	if (physical_devices.empty()) throw std::runtime_error("No suitable devices!");
	auto& physical_device = physical_devices[0];
	gpu = physical_device.device;
	graphics_queue_index = physical_device.graphics_queue_index;
	present_queue_index = physical_device.present_queue_index;
	surface_capabilities = physical_device.swapchain_info.capabilities;
	surface_format = physical_device.swapchain_info.formats;
	surface_present_modes = physical_device.swapchain_info.present_modes;
	gpu_features = physical_device.features;
	gpu_properties = physical_device.properties;

	device = CreateDevice(gpu, physical_device.graphics_queue_index, physical_device.present_queue_index);
	VULKAN_HPP_DEFAULT_DISPATCHER.init(device);

	allocator = CreateVmaAllocator(instance, gpu, device);

	graphics_queue = device.getQueue(graphics_queue_index, 0);
	present_queue = device.getQueue(present_queue_index, 0);

	temp_cmd_pool = std::make_unique<TemporaryCommandBufferPool>(device, graphics_queue_index);
}

DeviceContext::~DeviceContext()
{
	temp_cmd_pool->Destroy(device);
	temp_cmd_pool.reset();
	allocator.destroy();
	device.destroy();
	instance.destroySurfaceKHR(surface);
	debugger->Destroy(instance);
	debugger.reset();
	instance.destroy();
}
