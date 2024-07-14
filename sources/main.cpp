#include "Core/VulkanUsages.h"

#include <vector>
#include "Application/Systems.h"
#include "Application/Renderer/RendererApplication.h"
#include "Editor/EditorApplication.h"

void EditorMain() {
	EditorApplication editor;
	//Systems systems(renderer.GetRenderContext());
	entt::registry registry;
	while (editor.IsActive()) {
		//systems.Update(0.01);
		editor.Update(registry);
	}
}

void ApplicationMain() {
	RendererApplication renderer;
	Systems systems(renderer.GetRenderContext());

	while (renderer.IsActive()) {
		systems.Update(0.01);
		renderer.Update(systems.GetRegistry());
	}
}

int main() {
	try
	{
		EditorMain();
	}
	catch (const std::exception& e)
	{
		printf("%s\n", e.what());
	}

	return 0;
}

