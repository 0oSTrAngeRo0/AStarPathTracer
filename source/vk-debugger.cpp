#include "vulkan-app.h"

VKAPI_ATTR VkBool32 VKAPI_CALL
DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity, VkDebugUtilsMessageTypeFlagsEXT message_type,
	const VkDebugUtilsMessengerCallbackDataEXT *p_callback_data, void *p_user_data) {
	VulkanApp::OnDebug event = VulkanApp::OnDebug();
	event.message_severity = message_severity;
	event.message_type = message_type;
	event.p_callback_data = p_callback_data;
	event.p_user_data = p_user_data;
	EventRegistry<VulkanApp::OnDebug>::Broadcast(event);
	return VK_FALSE;
}

void VulkanApp::CreateDebugger(bool enable_debugger){
	if(!enable_debugger) return;
	VkDebugUtilsMessengerCreateInfoEXT create_info{};
	create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	create_info.messageSeverity =
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	create_info.messageType =
		VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	create_info.pfnUserCallback = DebugCallback;
	create_info.pUserData = nullptr; // Optional
	vkCreateDebugUtilsMessengerEXT(instance, &create_info, nullptr, &debugger);
}
