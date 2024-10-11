#include "Editor/EditorApplication.h"
#include "glm/glm.hpp"
#include "Core/DeviceContext.h"
#include "Core/Swapchain.h"
#include "Application/GlfwWindow.h"
#include "config.h"
#include "Editor/EditorRenderContext.h"
#include "Editor/EditorUI.h"
#include "Editor/EditorUIDrawer.h"
#include "Editor/EditorDeviceContextCreateConfig.h"

EditorApplication::EditorApplication() {
	AppConfig config = AppConfig::CreateDefault();
	config.window_title = "PathTracer Editor";
	window = std::make_unique<GlfwWindow>(config);

	EditorDeviceContextCreateConfig device_context_create_config(*window);
	context = std::make_unique<DeviceContext>(device_context_create_config);

	vk::SurfaceKHR surface = window->CreateWindowSurface(context->GetInstance(), nullptr).value();
	render_context = std::make_unique<EditorRenderContext>(*context, surface, window->GetActualExtent());
	ui = std::make_unique<EditorUI>(*context, *render_context, *window);
	ui_drawer = std::make_unique<EditorUIDrawer>();
	is_active = true;
}

void EditorApplication::Update(entt::registry& registry) {
	if (!is_active) return;
	is_active = window->IsActive();
	if (!is_active) return;
	bool need_recreate_swapchain = true;
	do {
		if (window->IsResized()) break;
		auto wait_result = render_context->WaitForNextFrame(*context);
		if (!wait_result.has_value()) break;
		auto& [frame, image_index] = wait_result.value();
		ui->UpdateBeginFrame();
		ui_drawer->DrawUI(registry);
		ui->UpdateRenderData();
		render_context->BeginFrame(*context, frame);
		render_context->CmdDrawUI(frame, *ui);
		render_context->SubmitFrame(*context, frame);
		ui->UpdateEndFrame();
		bool is_swapchain_bad = render_context->PresentFrame(*context, frame, image_index);
		render_context->EndFrame();
		if (!is_swapchain_bad) need_recreate_swapchain = false;
	} while (false);

	if (need_recreate_swapchain) {
		context->GetDevice().waitIdle();
		render_context->ResizeSwapchain(*context, window->GetActualExtent());
		return;
	}
}

EditorApplication::~EditorApplication() {
	is_active = false;
	context->GetDevice().waitIdle();
	ui->Destroy();
	render_context->Destroy(*context);
}
