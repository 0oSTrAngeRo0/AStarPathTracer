export module Core:Debugger;

import vulkan_hpp;

export class EngineDebugger {
private:
	vk::DebugUtilsMessengerEXT debugger;
public:
	EngineDebugger(const vk::Instance instance);
	void Destroy(const vk::Instance instance);
};