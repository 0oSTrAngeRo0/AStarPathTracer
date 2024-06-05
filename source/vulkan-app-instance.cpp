#include <stdexcept>
#include "vulkan-app.h"
#include "vulkan-utils.h"
#include "vulkan/vulkan.hpp"

std::vector<const char*> GetExtensions(const AppConfig& config) {
	std::vector<const char*> extensions{
			VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME,
	};
	extensions.insert(extensions.end(), config.external_extensions.begin(), config.external_extensions.end());
	if (config.enable_debug) {
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}
	return extensions;
}

std::vector<const char*> GetLayers(const AppConfig& config) {
	std::vector<const char*> layers{
	};
	if (config.enable_debug) {
		layers.push_back("VK_LAYER_KHRONOS_validation");
	}
	return layers;
}

void VulkanApp::CreateInstance() {
	VULKAN_HPP_DEFAULT_DISPATCHER.init();

	vk::ApplicationInfo app_info = vk::ApplicationInfo(config.window_title, vk::makeVersion(1, 0, 0), "AStarPathTracerEngine", vk::makeVersion(1, 3, 0), vk::ApiVersion13);
	std::vector<const char*> extensions = GetExtensions(config);
	std::vector<const char*> layers = GetLayers(config);
	vk::InstanceCreateInfo create_info = vk::InstanceCreateInfo(vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR, &app_info, layers, extensions);
	instance = vk::createInstance(create_info);
	VULKAN_HPP_DEFAULT_DISPATCHER.init(instance);
}
