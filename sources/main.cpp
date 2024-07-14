#include "Core/VulkanUsages.h"

#include "vulkan-app.h"
#include "main-window.h"
#include <vector>
#include "vulkan-utils.h"
#include "Application/Systems.h"
#include "Core/RenderContext.h"
#include "Engine/Resources/Resources.h"
#include "Editor/EditorApplication.h"

int main() {
	try
	{
		AppConfig config = AppConfig::CreateDefault();
		GlfwWindow main_window = GlfwWindow(config);
		DeviceContext context(main_window);
		RenderContext render(context);
		Systems systems(render);
		render.Update(context, systems.GetRegistry());

		VulkanApp vulkan_app(context, render);
		EditorApplication editor;

		while (!main_window.ShouldClose())
		{
			editor.Update();
			main_window.Update();
			systems.Update(0.01);
			render.Update(context, systems.GetRegistry());
			vulkan_app.Draw(context, render);
		}
		vulkan_app.Destroy(context);
		render.Destory(context);
	}
	catch (const std::exception& e)
	{
		printf("%s\n", e.what());
	}

	return 0;
}

