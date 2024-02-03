#ifndef PATHTRACER_VULKAN_UTILS_H
#define PATHTRACER_VULKAN_UTILS_H

//#include "volk.h"
#include "vulkan/vulkan.hpp"


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
