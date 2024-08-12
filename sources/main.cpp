#include "Core/VulkanUsages.h"

#include "Application/GlfwWindow.h"
#include <vector>
#include "Application/World.h"
#include "Application/Renderer/RendererApplication.h"
#include "Editor/EditorApplication.h"
#include "config.h"
#include "Engine/InputSystem.h"
#include "Engine/ShaderHostBuffer.h"
#include "Engine/Resources/ResourcesManager.h"

void EditorMain() {
	InputState input;
	GlfwWindow renderer_window(AppConfig::CreateDefault());
	renderer_window.RegisterInputState(input);
	RendererApplication renderer(static_cast<const VulkanWindow&>(renderer_window));
	EditorApplication editor;
	World world;

	world.GetRegistry().ctx().emplace<const InputState&>(input);
	world.GetRegistry().ctx().emplace<const HostShaderManager&>(HostShaderManager::GetInstance());
	while (editor.IsActive() && !renderer_window.ShouldClose()) {
		input.ClearFrameData();
		glfwPollEvents();
		world.Update(0.01);
		editor.Update(world.GetRegistry());
		renderer.Update(world.GetRegistry());
	}
}

void ApplicationMain() {
	InputState input;
	GlfwWindow renderer_window(AppConfig::CreateDefault());
	renderer_window.RegisterInputState(input);
	RendererApplication renderer(static_cast<const VulkanWindow&>(renderer_window));
	World world;

	world.GetRegistry().ctx().emplace<const InputState&>(input);
	world.GetRegistry().ctx().emplace<const HostShaderManager&>(HostShaderManager::GetInstance());
	while (!renderer_window.ShouldClose()) {
		input.ClearFrameData();
		glfwPollEvents();
		world.Update(0.01);
		renderer.Update(world.GetRegistry());
	}
}

int main() {
	ApplicationMain();

	//try
	//{
	//	ApplicationMain();
	//}
	//catch (const std::exception& e)
	//{
	//	printf("%s\n", e.what());
	//}

	return 0;
}

