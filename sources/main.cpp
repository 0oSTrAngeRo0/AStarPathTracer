#include "Core/VulkanUsages.h"

#include "Application/GlfwWindow.h"
#include <vector>
#include "Application/World.h"
#include "Application/Renderer/RendererApplication.h"
#include "Editor/EditorApplication.h"
#include "config.h"
#include "Engine/InputSystem.h"
#include "Engine/ShaderHostBuffer.h"
#include "Engine/HostShaderManager.h"
#include "Engine/Resources/ResourcesManager.h"

entt::registry CreateWorld(const InputState& input) {
	entt::registry registry;
	World::CreateDefault(registry);
	registry.ctx().emplace<const InputState&>(input);
	registry.ctx().emplace<const HostShaderManager&>(HostShaderManager::GetInstance());
	return registry;
}

void EditorMain() {
	InputState input;
	GlfwWindow renderer_window(AppConfig::CreateDefault());
	renderer_window.RegisterInputState(input);
	RendererApplication renderer(static_cast<const VulkanWindow&>(renderer_window));
	EditorApplication editor;
	entt::registry registry = CreateWorld(input);

	while (editor.IsActive() && !renderer_window.ShouldClose()) {
		input.ClearFrameData();
		glfwPollEvents();
		World::Update(registry, 0.01);
		editor.Update(registry);
		renderer.Update(registry);
	}
}

void ApplicationMain() {
	InputState input;
	GlfwWindow renderer_window(AppConfig::CreateDefault());
	renderer_window.RegisterInputState(input);
	RendererApplication renderer(static_cast<const VulkanWindow&>(renderer_window));
	entt::registry registry = CreateWorld(input);

	while (!renderer_window.ShouldClose()) {
		input.ClearFrameData();
		glfwPollEvents();
		World::Update(registry, 0.01);
		renderer.Update(registry);
	}
}

int main() {

	//try
	//{
		ApplicationMain();
		//EditorMain();
	//}
	//catch (const std::exception& e)
	//{
	//	printf("%s\n", e.what());
	//}

	return 0;
}

