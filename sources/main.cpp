#include "Core/VulkanUsages.h"

#include "Application/GlfwWindow.h"
#include "config.h"
#include <vector>
#include "Application/World.h"
#include "Application/Renderer/RendererApplication.h"
#include "Engine/InputSystem.h"
#include "Engine/ShaderHostBuffer.h"
#include "Engine/HostShaderManager.h"
#include "Engine/Resources/ResourcesManager.h"
#include "Editor/EditorApplication.h"

entt::registry CreateWorld(const InputState& input) {
	entt::registry registry;
	World::CreateDefault(registry);
	registry.ctx().emplace<const InputState&>(input);
	registry.ctx().emplace<HostShaderManager&>(HostShaderManager::GetInstance());
	registry.ctx().emplace<const ResourcesManager&>(ResourcesManager::GetInstance());
	return registry;
}

std::unique_ptr<GlfwWindow> CreateWindow(std::shared_ptr<InputState> input) {
	const auto& config = AppConfig::CreateDefault();
	std::unique_ptr<GlfwWindow> window = std::make_unique<GlfwWindow>(config);
	window->RegisterInputState(std::move(input));
	return window;
}

void EditorMain() {
	std::shared_ptr<InputState> input = std::make_shared<InputState>();
	std::unique_ptr<VulkanWindow> window = CreateWindow(input);
	RendererApplication renderer(std::move(window));
	entt::registry registry = CreateWorld(*input);
	EditorApplication editor;

	while (renderer.IsActive() && editor.IsActive()) {
		input->ClearFrameData();
		GlfwWindow::PollEvents();
		World::Update(registry, 0.01);
		if (renderer.IsActive()) {
			renderer.Update(registry);
		}
		if (editor.IsActive()) {
			editor.Update(registry);
		}
	}
}

void ApplicationMain() {
	std::shared_ptr<InputState> input = std::make_shared<InputState>();
	std::unique_ptr<VulkanWindow> window = CreateWindow(input);
	RendererApplication renderer(std::move(window));
	entt::registry registry = CreateWorld(*input);

	while (renderer.IsActive()) {
		input->ClearFrameData();
		GlfwWindow::PollEvents();
		World::Update(registry, 0.01);
		renderer.Update(registry);
	}
}

int main() {
	try {
		//ApplicationMain();
		EditorMain();
	}
	catch (const std::exception& e) {
		printf("%s\n", e.what());
	}

	return 0;
}

