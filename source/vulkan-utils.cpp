#include "vulkan-utils.h"
#include <vector>
#include <cstdio>

void vkext::PrintInstanceExtensions() {
    uint32_t extension_count = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);
    std::vector<VkExtensionProperties> extensions(extension_count);
    vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extensions.data());
    for (const VkExtensionProperties &extension: extensions) {
        uint32_t version = extension.specVersion;
        uint32_t variant = VK_API_VERSION_VARIANT(version);
        uint32_t major = VK_API_VERSION_MAJOR(version);
        uint32_t minor = VK_API_VERSION_MINOR(version);
        uint32_t patch = VK_API_VERSION_PATCH(version);
        printf("%s\t%d.%d.%d.%d\n", extension.extensionName, variant, major, minor, patch);
    }
}

void vkext::PrintDeviceExtensions(const VkPhysicalDevice& device) {
    uint32_t extension_count = 0;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, nullptr);
    std::vector<VkExtensionProperties> extensions(extension_count);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, extensions.data());

    for (const VkExtensionProperties &extension: extensions) {
        uint32_t version = extension.specVersion;
        uint32_t variant = VK_API_VERSION_VARIANT(version);
        uint32_t major = VK_API_VERSION_MAJOR(version);
        uint32_t minor = VK_API_VERSION_MINOR(version);
        uint32_t patch = VK_API_VERSION_PATCH(version);
        printf("%s\t%d.%d.%d.%d\n", extension.extensionName, variant, major, minor, patch);
    }
}

void vkext::PrintInstanceLayers() {
    uint32_t layer_count = 0;
    vkEnumerateInstanceLayerProperties(&layer_count, nullptr);
    std::vector<VkLayerProperties> layers(layer_count);
    vkEnumerateInstanceLayerProperties(&layer_count, layers.data());
    for (const VkLayerProperties &layer: layers) {
        uint32_t version = layer.specVersion;
        uint32_t variant = VK_API_VERSION_VARIANT(version);
        uint32_t major = VK_API_VERSION_MAJOR(version);
        uint32_t minor = VK_API_VERSION_MINOR(version);
        uint32_t patch = VK_API_VERSION_PATCH(version);
        printf("%s\t%d.%d.%d.%d\n", layer.layerName, variant, major, minor, patch);
    }
}