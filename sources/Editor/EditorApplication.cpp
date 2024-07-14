#include "Editor/EditorApplication.h"
#include "glm/glm.hpp"
#include "Core/DeviceContext.h"
#include "Core/Swapchain.h"
#include "config.h"
#include "Application/GlfwWindow.h"
#include "Editor/EditorRenderContext.h"
#include "Editor/EditorUI.h"
#include "Editor/EditorUIDrawer.h"

EditorApplication::EditorApplication() {
	AppConfig config = AppConfig::CreateDefault();
	config.window_title = "PathTracer Editor";
	window = std::make_unique<GlfwWindow>(config);
	context = std::make_unique<DeviceContext>(*window);
	render_context = std::make_unique<EditorRenderContext>(*context);
	ui = std::make_unique<EditorUI>(*context, *render_context, *window);
	ui_drawer = std::make_unique<EditorUIDrawer>();
	is_active = true;
}

void EditorApplication::Update(entt::registry& registry) {
	if (!is_active) return;
	is_active = !window->ShouldClose();

	window->Update();
	ui->UpdateBeginFrame();
	ui_drawer->DrawUI(registry);
	ui->UpdateRenderData();
	auto [frame, image_index] = render_context->BeginFrame(*context);
	render_context->CmdDrawUI(frame, *ui);
	render_context->SubmitFrame(*context, frame);
	ui->UpdateEndFrame();
	render_context->PresentFrame(*context, frame, image_index);
	render_context->EndFrame();
}

EditorApplication::~EditorApplication() {
	is_active = false;
	context->GetDevice().waitIdle();
	ui->Destroy();
	render_context->Destroy(*context);
}
