#include "vulkan-app.h"
#include "main-window.h"
#include <vector>
#include "vulkan-utils.h"

int main() {
	try
	{
		AppConfig config = AppConfig::CreateDefault();

		std::unique_ptr<GlfwWindow> main_window = std::make_unique<GlfwWindow>(config);

		config.external_extensions = main_window->GetVulkanExtensions();

		std::unique_ptr<VulkanApp> vulkan_app = std::make_unique<VulkanApp>(config, *main_window);

		while (!main_window->ShouldClose())
		{
			if (!main_window->ShouldClose()) {
				main_window->Update();
				vulkan_app->Update();
			}
			else
			{
				vulkan_app.reset();
				main_window.reset();
			}
		}
	}
	catch (const std::exception& e)
	{
		printf("%s\n", e.what());
	}

	return 0;
}

