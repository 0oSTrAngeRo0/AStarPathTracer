#include <stdexcept>
#include "vulkan-app.h"
#include "vulkan-utils.h"

std::vector<const char *> GetExtensions(const AppConfig &config);

std::vector<const char *> GetLayers(const AppConfig &config);

VKAPI_ATTR VkBool32 VKAPI_CALL
DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity, VkDebugUtilsMessageTypeFlagsEXT message_type,
              const VkDebugUtilsMessengerCallbackDataEXT *p_callback_data, void *p_user_data);

VkDebugUtilsMessengerCreateInfoEXT GetDebugCreateInfo();

void VulkanApp::CreateInstance() {
    VkApplicationInfo app_info{};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = config.window_title;
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "No Engine";
    app_info.engineVersion = VK_MAKE_VERSION(1, 3, 0);
    app_info.apiVersion = VK_API_VERSION_1_3;
    app_info.pNext = nullptr;

    std::vector<const char *> extensions = GetExtensions(config);
    std::vector<const char *> layers = GetLayers(config);

    VkInstanceCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
    create_info.pApplicationInfo = &app_info;
    create_info.enabledExtensionCount = extensions.size();
    create_info.ppEnabledExtensionNames = extensions.data();
    create_info.enabledLayerCount = layers.size();
    create_info.ppEnabledLayerNames = layers.data();
    create_info.pNext = nullptr;
    VkResult result = vkCreateInstance(&create_info, nullptr, &instance);
    if (result != VkResult::VK_SUCCESS) {
        throw std::runtime_error("Failed to create instance!");
    }
}

std::vector<const char *> GetExtensions(const AppConfig &config) {
    std::vector<const char *> extensions{
            VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME,
    };
    extensions.insert(extensions.end(), config.external_extensions.begin(), config.external_extensions.end());
    if (config.enable_debug) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }
    return extensions;
}

std::vector<const char *> GetLayers(const AppConfig &config) {
    std::vector<const char *> layers{
    };
    if (config.enable_debug) {
        layers.push_back("VK_LAYER_KHRONOS_validation");
    }
    return layers;
}
