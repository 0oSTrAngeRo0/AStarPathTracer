#include "Editor/EditorApplication.h"
#include "glm/glm.hpp"
#include "Core/DeviceContext.h"
#include "Core/Swapchain.h"
#include "config.h"
#include "main-window.h"
#include "Editor/EditorRenderContext.h"
#include "Editor/EditorUI.h"

EditorApplication::EditorApplication() {
	window = std::make_unique<GlfwWindow>(AppConfig::CreateDefault());
	context = std::make_unique<DeviceContext>(*window);
	render_context = std::make_unique<EditorRenderContext>(*context);
	ui = std::make_unique<EditorUI>(*context, *render_context, *window);
}

void EditorApplication::Update() {
	if (window->ShouldClose()) return;
	window->Update();
	ui->UpdateBeginFrame();
	auto [frame, image_index] = render_context->BeginFrame(*context);
	render_context->CmdDrawUI(frame, *ui);
	render_context->SubmitFrame(*context, frame);
	ui->UpdateEndFrame();
	render_context->PresentFrame(*context, frame, image_index);
	render_context->EndFrame();
}

EditorApplication::~EditorApplication() {
	context->GetDevice().waitIdle();
	context->~DeviceContext();
}
