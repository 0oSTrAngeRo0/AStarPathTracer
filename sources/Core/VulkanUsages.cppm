#include <vulkan/vulkan_hpp_macros.hpp>

export module Core:VulkanUsages;

import vulkan_hpp;
import std;

#define VMA_LEAK_LOG_FORMAT(format, ...) do { \
	printf((format), __VA_ARGS__); \
	printf("\n"); \
} while(false)

#if VULKAN_HPP_DISPATCH_LOADER_DYNAMIC == 1
export VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE
#endif