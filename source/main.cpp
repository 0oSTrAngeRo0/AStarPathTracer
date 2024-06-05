#include "main-window.h"
#include "vulkan-app.h"
#include "debugger.h"
#include <vector>
#include "vulkan-utils.h"

int main() {

	AppConfig config = AppConfig::CreateDefault();

	Debugger* debugger = new Debugger();
	EventRegistry<VulkanApp::OnDebug>::Register(debugger);

	MainWindow* window = new MainWindow();
	window->Create(config);

	config.external_extensions = window->GetVulkanExtensions();

	VulkanApp* vulkan_app = new VulkanApp();
	vulkan_app->Create(config, window);
	EventRegistry<MainWindow::OnDestroyed>::Register(vulkan_app);
	EventRegistry<MainWindow::OnDrawFrame>::Register(vulkan_app);

    //vkext::PrintInstanceExtensions();

	window->Run();

	return 0;
}

