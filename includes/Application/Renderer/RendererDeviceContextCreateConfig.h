#pragma once

import Core:DeviceContext;

class RendererDeviceContextCreateInfo : public DeviceContextCreateConfig {
public:
	RendererDeviceContextCreateInfo(VulkanWindow& window) : window(window) {
		instance_extensions = {
			VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME,
		};

		auto window_extensions = window.GetVulkanExtensions();
		instance_extensions.insert(instance_extensions.begin(), window_extensions.begin(), window_extensions.end());

		instance_layers = {};
		gpu_extensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME,

			VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME,
			VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME,

			VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,
			VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,
			VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME,

			VK_KHR_SPIRV_1_4_EXTENSION_NAME,

			VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME,
			VK_EXT_ROBUSTNESS_2_EXTENSION_NAME,
			VK_EXT_SCALAR_BLOCK_LAYOUT_EXTENSION_NAME, 
		};
	}
	std::vector<std::string> GetInstanceExtensions() const override { return instance_extensions; }
	std::vector<std::string> GetInstanceLayers() const override { return instance_layers; }
	std::vector<std::string> GetGpuExtensions() const override { return gpu_extensions; }
	vk::PhysicalDeviceFeatures2 GetGpuFeatures() const override { return gpu_features.get(); }
	bool OtherGpuCheck(vk::PhysicalDevice gpu) const override { return true; }
private:
	std::vector<std::string> instance_extensions;
	std::vector<std::string> instance_layers;
	std::vector<std::string> gpu_extensions;
	vk::StructureChain<
		vk::PhysicalDeviceFeatures2,
		vk::PhysicalDeviceBufferDeviceAddressFeatures,
		vk::PhysicalDeviceDynamicRenderingFeatures,
		vk::PhysicalDeviceSynchronization2Features,
		vk::PhysicalDeviceDescriptorIndexingFeatures,
		vk::PhysicalDeviceMaintenance4Features,
		vk::PhysicalDeviceAccelerationStructureFeaturesKHR,
		vk::PhysicalDeviceRayTracingPipelineFeaturesKHR,
		vk::PhysicalDeviceRobustness2FeaturesEXT,
		vk::PhysicalDevice16BitStorageFeaturesKHR,
		vk::PhysicalDeviceScalarBlockLayoutFeatures
	> gpu_features;
	vk::SurfaceKHR surface;
	VulkanWindow& window;
};