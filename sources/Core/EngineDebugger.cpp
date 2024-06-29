#include "Core/EngineDebugger.h"

VKAPI_ATTR VkBool32 VKAPI_CALL
DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity, VkDebugUtilsMessageTypeFlagsEXT message_type,
    const VkDebugUtilsMessengerCallbackDataEXT* p_callback_data, void* p_user_data) {
    std::string severity = vk::to_string(vk::DebugUtilsMessageSeverityFlagsEXT(message_severity));
    severity = severity.substr(2, severity.size() - 4);
    std::string type = vk::to_string(vk::DebugUtilsMessageTypeFlagsEXT(message_type));
    type = type.substr(2, type.size() - 4);
    printf("[%s][%s] %s\n\n", severity.data(), type.data(), p_callback_data->pMessage);
    return VK_FALSE;
}

EngineDebugger::EngineDebugger(const vk::Instance instance)
{
    vk::DebugUtilsMessageSeverityFlagsEXT severity =
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;

    vk::DebugUtilsMessageTypeFlagsEXT type =
        vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
        vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
        vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
        vk::DebugUtilsMessageTypeFlagBitsEXT::eDeviceAddressBinding;

    vk::DebugUtilsMessengerCreateInfoEXT create_info({}, severity, type, DebugCallback);
    debugger = instance.createDebugUtilsMessengerEXT(create_info);
}

void EngineDebugger::Destroy(const vk::Instance instance)
{
    instance.destroyDebugUtilsMessengerEXT(debugger);
}
