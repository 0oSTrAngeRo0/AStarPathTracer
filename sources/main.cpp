#include "vulkan-app.h"
#include "main-window.h"
#include <vector>
#include "vulkan-utils.h"

int main() {

	try
	{
		AppConfig config = AppConfig::CreateDefault();

		MainWindow* window = new MainWindow();
		window->Create(config);

		config.external_extensions = window->GetVulkanExtensions();

		VulkanApp* vulkan_app = new VulkanApp(config, window);
		EventRegistry<MainWindow::OnDestroyed>::Register(vulkan_app);
		EventRegistry<MainWindow::OnDrawFrame>::Register(vulkan_app);

		//vkext::PrintInstanceExtensions();

		window->Run();
	}
	catch (const std::exception& e)
	{
		printf("%s\n", e.what());
	}

	return 0;
}

