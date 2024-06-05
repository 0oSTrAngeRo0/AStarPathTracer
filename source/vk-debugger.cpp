#include "vulkan-app.h"

VKAPI_ATTR VkBool32 VKAPI_CALL
DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity, VkDebugUtilsMessageTypeFlagsEXT message_type,
	const VkDebugUtilsMessengerCallbackDataEXT* p_callback_data, void* p_user_data) {
	VulkanApp::OnDebug event = VulkanApp::OnDebug();
	event.message_severity = message_severity;
	event.message_type = message_type;
	event.p_callback_data = p_callback_data;
	event.p_user_data = p_user_data;
	EventRegistry<VulkanApp::OnDebug>::Broadcast(event);
	return VK_FALSE;
}

void VulkanApp::CreateDebugger(bool enable_debugger) {
	if (!enable_debugger) return;

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
