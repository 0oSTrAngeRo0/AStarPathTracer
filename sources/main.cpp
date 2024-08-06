#include "Core/VulkanUsages.h"

#include <vector>
#include "Application/Systems.h"
#include "Application/Renderer/RendererApplication.h"
#include "Editor/EditorApplication.h"

void EditorMain() {
	RendererApplication renderer;
	EditorApplication editor;
	Systems systems;
	while (editor.IsActive() && renderer.IsActive()) {
		systems.Update(0.01);
		editor.Update(systems.GetRegistry());
		renderer.Update(systems.GetRegistry());
	}
}

void ApplicationMain() {
	RendererApplication renderer;
	Systems systems;

	while (renderer.IsActive()) {
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

