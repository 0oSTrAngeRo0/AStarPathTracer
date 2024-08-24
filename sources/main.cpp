#include "Core/VulkanUsages.h"

#include "Application/GlfwWindow.h"
#include "config.h"
#include "Editor/EditorApplication.h"
#include <vector>
#include "Application/World.h"
#include "Application/Renderer/RendererApplication.h"
#include "Engine/InputSystem.h"
#include "Engine/ShaderHostBuffer.h"
#include "Engine/HostShaderManager.h"
#include "Engine/Resources/ResourcesManager.h"
#include "Application/NsightAftermath/NsightAftermathGpuCrashTracker.h"

entt::registry CreateWorld(const InputState& input) {
	entt::registry registry;
	World::CreateDefault(registry);
	registry.ctx().emplace<const InputState&>(input);
	registry.ctx().emplace<const HostShaderManager&>(HostShaderManager::GetInstance());
	return registry;
}

std::unique_ptr<GlfwWindow> CreateWindow(std::shared_ptr<InputState> input) {
	const auto& config = AppConfig::CreateDefault();
	std::unique_ptr<GlfwWindow> window = std::make_unique<GlfwWindow>(config);
	window->RegisterInputState(std::move(input));
	return window;
}

//void EditorMain() {
//	std::shared_ptr<InputState> input = std::make_shared<InputState>();
//	RendererApplication renderer(std::move(CreateWindow(input)));
//	EditorApplication editor;
//	entt::registry registry = CreateWorld(*input);
//
//	while (editor.IsActive() && renderer.IsActive()) {
//		GlfwWindow::PollEvents();
//		input->ClearFrameData();
//		World::Update(registry, 0.01);
//		editor.Update(registry);
//		renderer.Update(registry);
//	}
//}

void ApplicationMain() {
	std::shared_ptr<InputState> input = std::make_shared<InputState>();
	std::unique_ptr<VulkanWindow> window = CreateWindow(input);
	RendererApplication renderer(std::move(window));
	entt::registry registry = CreateWorld(*input);

	while (renderer.IsActive()) {
		GlfwWindow::PollEvents();
		World::Update(registry, 0.01);
		renderer.Update(registry);
	}
}

int main() {

	try
	{
		//GpuCrashTracker::MarkerMap marker_map;
		//GpuCrashTracker profiler(marker_map);
		//profiler.Initialize();

		ApplicationMain();
		//EditorMain();
	}
	catch (const std::exception& e)
	{
		printf("%s\n", e.what());
	}

	return 0;
}

