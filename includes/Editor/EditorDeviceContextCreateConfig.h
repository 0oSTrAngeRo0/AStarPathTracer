#pragma once

#include "Core/DeviceContext.h"

class EditorDeviceContextCreateConfig : public DeviceContextCreateConfig {
public:
	EditorDeviceContextCreateConfig(VulkanWindow& window) : window(window) {
		instance_extensions = {
			VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME,

			#if defined(ENABLE_DEBUG)
			VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
			#endif // ENABLE_DEBUG
		};

		auto window_extensions = window.GetVulkanExtensions();
		instance_extensions.insert(instance_extensions.begin(), window_extensions.begin(), window_extensions.end());

		instance_layers = {
			#if defined(ENABLE_DEBUG)
			"VK_LAYER_KHRONOS_validation"
			#endif // ENABLE_DEBUG
		};
		gpu_extensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		};
	}
	std::vector<std::string> GetInstanceExtensions() const override { return instance_extensions; }
	std::vector<std::string> GetInstanceLayers() const override { return instance_layers; }
	std::vector<std::string> GetGpuExtensions() const override { return gpu_extensions; }
	vk::PhysicalDeviceFeatures2 GetGpuFeatures() const override { return gpu_features.get(); }
	bool OtherGpuCheck(vk::PhysicalDevice gpu) const override { return true; }
	vk::SurfaceKHR CreateSurface(vk::Instance instance) override {
		if (!surface) {
			surface = window.CreateWindowSurface(instance, nullptr).value();
		}
		std::printf("Create Surface");
		return surface;
	}
	std::optional<vk::SurfaceKHR> GetSurface() const override {
		if (surface) return std::make_optional(surface);
		return std::nullopt;
	}
private:
	std::vector<std::string> instance_extensions;
	std::vector<std::string> instance_layers;
	std::vector<std::string> gpu_extensions;
	vk::StructureChain<vk::PhysicalDeviceFeatures2> gpu_features;
	vk::SurfaceKHR surface;
	VulkanWindow& window;
};