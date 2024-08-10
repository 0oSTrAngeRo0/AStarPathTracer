#include "Core/VulkanUsages.h"

#include "Application/GlfwWindow.h"
#include <vector>
#include "Application/Systems.h"
#include "Application/Renderer/RendererApplication.h"
#include "Editor/EditorApplication.h"
#include "config.h"
#include "Engine/InputSystem.h"

void EditorMain() {
	InputState input;
	GlfwWindow renderer_window(AppConfig::CreateDefault());
	renderer_window.RegisterInputState(input);
	RendererApplication renderer(static_cast<const VulkanWindow&>(renderer_window));
	EditorApplication editor;
	Systems systems;
	while (editor.IsActive() && !renderer_window.ShouldClose()) {
		input.ClearFrameData();
		glfwPollEvents();
		systems.Update(0.01);
		editor.Update(systems.GetRegistry());
		renderer.Update(systems.GetRegistry());
	}
}

void ApplicationMain() {
	InputState input;
	GlfwWindow renderer_window(AppConfig::CreateDefault());
	renderer_window.RegisterInputState(input);
	RendererApplication renderer(static_cast<const VulkanWindow&>(renderer_window));
	Systems systems;

	while (!renderer_window.ShouldClose()) {
		input.ClearFrameData();
		glfwPollEvents();
		systems.Update(0.01);
		renderer.Update(systems.GetRegistry());
	}
}

int main() {
	try
	{
		ApplicationMain();
	}
	catch (const std::exception& e)
	{
		printf("%s\n", e.what());
	}

	return 0;
}

