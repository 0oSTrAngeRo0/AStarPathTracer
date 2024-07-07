#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#include <vulkan/vulkan.hpp>
#define VMA_IMPLEMENTATION 
#define VMA_LEAK_LOG_FORMAT(format, ...) do { \
	printf((format), __VA_ARGS__); \
	printf("\n"); \
} while(false)
#include <vk_mem_alloc.hpp>

#include "Core/VulkanUsages.h"

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE