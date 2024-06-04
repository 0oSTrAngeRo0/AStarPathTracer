#ifndef PATHTRACER_VULKAN_UTILS_H
#define PATHTRACER_VULKAN_UTILS_H

//#include "volk.h"
#include "vulkan/vulkan.hpp"
#include "vulkan/vk_enum_string_helper.h"
#include "sstream"

#define DO_WHILE_WRAPPER(inner) do { \
    inner \
} while(0)

#define VK_CHECK(function) DO_WHILE_WRAPPER( \
    VkResult result = function; \
    if(result == VK_SUCCESS) break;          \
	std::ostringstream stream;                  \
	stream << "Vulkan error on [" << #function << "] with error code [" << string_VkResult(result) << "]\n"; \
	stream << __FILE__ << " (at " << __FUNCTION__ << ":" << __LINE__ << ")\n\n"; \
    throw std::runtime_error(stream.str().data()); \
)

#define THROW(error) DO_WHILE_WRAPPER( \
	std::ostringstream stream;  \
	stream << error << '\n' << __FILE__ << " (at " << __FUNCTION__ << ":" << __LINE__ << ")\n\n"; \
    throw std::runtime_error(stream.str().data()); \
)

namespace vkext {
    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                          const VkAllocationCallbacks *pAllocator,
                                          VkDebugUtilsMessengerEXT *pDebugMessenger);

    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
                                       const VkAllocationCallbacks *pAllocator);

    void PrintInstanceExtensions();
    void PrintDeviceExtensions(const VkPhysicalDevice& device);
    void PrintInstanceLayers();
}

#endif //PATHTRACER_VULKAN_UTILS_H
