#include "debugger.h"
#include <cstdio>

void Debugger::OnExecute(VulkanApp::OnDebug event) {
    const char *message_severity;
    switch (event.message_type) {
        case VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            message_severity = "Verbose";
            break;
        case VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            message_severity = "Info";
            break;
        case VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            message_severity = "Warning";
            break;
        case VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            message_severity = "Error";
            break;
        default:
            message_severity = "Unknown";
            break;
    }

    const char *message_type;
    switch (event.message_type) {
        case VkDebugUtilsMessageTypeFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
            message_type = "Validation";
            break;
        case VkDebugUtilsMessageTypeFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
            message_type = "General";
            break;
        case VkDebugUtilsMessageTypeFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
            message_type = "Performance";
            break;
        case VkDebugUtilsMessageTypeFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT:
            message_type = "DeviceAddressBinding";
            break;
        default:
            message_type = "Unknown";
            break;
    }
    printf("[%s][%s] %s\n", message_severity, message_type, event.p_callback_data->pMessage);
}
