#include "Core/DeviceContext.h"

#define APP_NAME "AStarPathTracer"
#define ENGINE_NAME "AStarPathTracerEngine"

#if defined(ENABLE_DEBUG)
#include "Core/EngineDebugger.h"
#endif // ENABLE_DEBUG

#include "Core/Window.h"
#include "Utilities/Algorithm.h"
#include <unordered_set>

vk::Instance DeviceContext::CreateInstance(const DeviceContextCreateConfig& config) {
	vk::ApplicationInfo app_info = vk::ApplicationInfo(APP_NAME, vk::makeVersion(1, 0, 0), ENGINE_NAME, vk::makeVersion(1, 3, 0), vk::ApiVersion13);

	auto str_extensions = config.GetInstanceExtensions();
	#if defined(ENABLE_DEBUG)
	str_extensions.emplace_back(std::string(VK_EXT_DEBUG_UTILS_EXTENSION_NAME));
	#endif
	std::vector<const char*> extensions(str_extensions.size());
	std::transform(str_extensions.begin(), str_extensions.end(), extensions.begin(), [](auto& extension) { return extension.c_str(); });

	auto str_layers = config.GetInstanceLayers();
	#if defined(ENABLE_DEBUG)
	str_layers.emplace_back(std::string("VK_LAYER_KHRONOS_validation"));
	#endif
	std::vector<const char*> layers(str_layers.size());
	std::transform(str_layers.begin(), str_layers.end(), layers.begin(), [](auto& layer) { return layer.c_str(); });

	vk::InstanceCreateInfo create_info = vk::InstanceCreateInfo(vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR, &app_info, layers, extensions);
	return vk::createInstance(create_info);
}

std::optional<DeviceContext::QueueIndices> DeviceContext::GetSuitableQueueFamilies(vk::PhysicalDevice device) {
	auto properties = device.getQueueFamilyProperties();
	bool contains_graphics = false;
	bool contains_present = false;
	QueueIndices indices;
	for (int i = 0, end = properties.size(); i < end; ++i) {
		const vk::QueueFamilyProperties& property = properties[i];
		if (property.queueFlags & vk::QueueFlagBits::eGraphics) {
			indices.graphics = i;
			contains_graphics = true;

			indices.present = i;
			contains_present = true;
		}
	}
	return (contains_graphics && contains_present) ? std::make_optional(indices) : std::nullopt;
}

std::optional<vk::PhysicalDevice> DeviceContext::PickPhysicalDevice(vk::Instance instance, const DeviceContextCreateConfig& config) {
	auto gpus = instance.enumeratePhysicalDevices();

	const auto extensions = config.GetGpuExtensions();
	std::remove_if(gpus.begin(), gpus.end(), [&extensions = extensions](auto gpu) { return !IsGpuExtensionsValid(gpu, extensions); });

	std::remove_if(gpus.begin(), gpus.end(), [](auto gpu) { return !GetSuitableQueueFamilies(gpu); });

	std::remove_if(gpus.begin(), gpus.end(), [&config = config](auto gpu) { return config.OtherGpuCheck(gpu); });

	if (gpus.size() == 0) return std::nullopt;
	return gpus[0];
}

bool DeviceContext::IsGpuExtensionsValid(vk::PhysicalDevice gpu, std::vector<std::string> extensions) {
	auto avaliable = gpu.enumerateDeviceExtensionProperties();
	std::vector<std::string> avaliable_str(avaliable.size());
	std::transform(avaliable.begin(), avaliable.end(), avaliable_str.begin(), [](auto& extension) { return std::string(extension.extensionName.data()); });
	std::unordered_set<std::string> set(avaliable_str.begin(), avaliable_str.end());
	for (const auto& extension : extensions) {
		if (set.contains(extension)) continue;
		return false;
	}
	return true;
}

vk::Device DeviceContext::CreateDevice(const vk::PhysicalDevice gpu, const DeviceContextCreateConfig& config, const QueueIndices& queue_indices) {
	std::vector<float> queue_priorities = { 1.0f };
	std::vector<vk::DeviceQueueCreateInfo> queue_create_infos = {
		vk::DeviceQueueCreateInfo({}, queue_indices.graphics, queue_priorities, {}),
		vk::DeviceQueueCreateInfo({}, queue_indices.present, queue_priorities, {})
	};
	std::sort(queue_create_infos.begin(), queue_create_infos.end());
	queue_create_infos.erase(std::unique(queue_create_infos.begin(), queue_create_infos.end()), queue_create_infos.end());

	auto str_extensions = config.GetGpuExtensions();
	std::vector<const char*> extensions(str_extensions.size());
	std::transform(str_extensions.begin(), str_extensions.end(), extensions.begin(), [](auto& extension) { return extension.c_str(); });

	auto features = config.GetGpuFeatures();
	gpu.getFeatures2(&features);

	vk::DeviceCreateInfo device_info({}, queue_create_infos, {}, extensions, nullptr, &features);

	#if defined(ENABLE_NSIGHT_AFTERMATH)
	vk::DeviceDiagnosticsConfigCreateInfoNV device_diagnostics_config_create_info(
		vk::DeviceDiagnosticsConfigFlagBitsNV::eEnableAutomaticCheckpoints
		| vk::DeviceDiagnosticsConfigFlagBitsNV::eEnableResourceTracking
		| vk::DeviceDiagnosticsConfigFlagBitsNV::eEnableShaderDebugInfo
		| vk::DeviceDiagnosticsConfigFlagBitsNV::eEnableShaderErrorReporting
	);
	#endif

	vk::StructureChain<
		vk::DeviceCreateInfo
		#if defined(ENABLE_NSIGHT_AFTERMATH)
		, vk::DeviceDiagnosticsConfigCreateInfoNV
		#endif // defined(ENABLE_NSIGHT_AFTERMATH)
	> device_create_info_chain(
		device_info
		#if defined(ENABLE_NSIGHT_AFTERMATH)
		, device_diagnostics_config_create_info
		#endif // defined(ENABLE_NSIGHT_AFTERMATH)
	);

	return gpu.createDevice(device_create_info_chain.get());
}

vma::Allocator DeviceContext::CreateVmaAllocator(const vk::Instance instance, const vk::PhysicalDevice gpu, const vk::Device device) {
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

DeviceContext::DeviceContext(DeviceContextCreateConfig& config) {
	VULKAN_HPP_DEFAULT_DISPATCHER.init();
	instance = CreateInstance(config);
	VULKAN_HPP_DEFAULT_DISPATCHER.init(instance);

#if defined(ENABLE_DEBUG)
	debugger.reset(new EngineDebugger(instance));
#endif // ENABLE_DEBUG

	auto gpu_result = PickPhysicalDevice(instance, config);
	if (!gpu_result)
		throw std::runtime_error("Failed to find a valid physical device");
	gpu = gpu_result.value();
	auto&& queue_indices = GetSuitableQueueFamilies(gpu).value();

	graphics_queue_index = queue_indices.graphics;
	present_queue_index = queue_indices.present;

	device = CreateDevice(gpu, config, queue_indices);
	VULKAN_HPP_DEFAULT_DISPATCHER.init(device);

	allocator = CreateVmaAllocator(instance, gpu, device);

	graphics_queue = device.getQueue(graphics_queue_index, 0);
	present_queue = device.getQueue(present_queue_index, 0);

	temp_cmd_pool = std::make_unique<TemporaryCommandBufferPool>(device, graphics_queue_index);
}

DeviceContext::~DeviceContext() {
	temp_cmd_pool->Destroy(device);
	temp_cmd_pool.reset();
	allocator.destroy();
	device.destroy();
	debugger->Destroy(instance);
	debugger.reset();
	instance.destroy();
}
