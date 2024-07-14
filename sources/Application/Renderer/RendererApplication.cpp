#include "config.h"
#include "Application/GlfwWindow.h"
#include "Core/DeviceContext.h"
#include "Application/Renderer/RenderContext.h"
#include "Application/Renderer/Renderer.h"

#include "Application/Renderer/RendererApplication.h"


RendererApplication::RendererApplication() {
	AppConfig config = AppConfig::CreateDefault();
	window = std::make_unique<GlfwWindow>(config);
	context = std::make_unique<DeviceContext>(*window);
	render_context = std::make_unique<RenderContext>(*context);
	renderer = std::make_unique<Renderer>(*context, *render_context);
	is_active = true;
}

void RendererApplication::Update(entt::registry& registry) {
	if (!is_active) return;
	is_active = !window->ShouldClose();

	window->Update();
	render_context->Update(*context, registry);
	renderer->Draw(*context, *render_context);
}

RendererApplication::~RendererApplication() {
	is_active = false;
	render_context->Destory(*context);
	renderer->Destroy(*context);
	context.reset();
}
