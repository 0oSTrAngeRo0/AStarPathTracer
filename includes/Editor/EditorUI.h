#pragma once

#include <vulkan/vulkan.hpp>

class DeviceContext;
class EditorRenderContext;
class GlfwWindow;

class EditorUI {
private:
	static void CheckVkResult(VkResult err);
public:
	EditorUI(const DeviceContext& context, const EditorRenderContext& render_context, const GlfwWindow& window);
	void UpdateBeginFrame();
	void CmdDraw(vk::CommandBuffer cmd);
	void UpdateEndFrame();
	~EditorUI();
};