#pragma once

#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1

#include <vulkan/vulkan.hpp>

class EngineDebugger {
private:
	vk::DebugUtilsMessengerEXT debugger;
public:
	EngineDebugger(const vk::Instance instance);
	void Destroy(const vk::Instance instance);
};