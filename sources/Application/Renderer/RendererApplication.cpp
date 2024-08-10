#include "config.h"
#include "Application/GlfwWindow.h"
#include "Core/DeviceContext.h"
#include "Application/Renderer/RenderContext.h"
#include "Application/Renderer/Renderer.h"

#include "Application/Renderer/RendererApplication.h"


RendererApplication::RendererApplication(const VulkanWindow& window) {
	AppConfig config = AppConfig::CreateDefault();
	config.window_title = "Path Tracer Renderer";
	context = std::make_unique<DeviceContext>(window);
	render_context = std::make_unique<RenderContext>(*context);
	renderer = std::make_unique<Renderer>(*context, *render_context);
}

void RendererApplication::Update(entt::registry& registry) {
	render_context->Update(*context, registry);
	renderer->Draw(*context, *render_context);
}

RendererApplication::~RendererApplication() {
	render_context->Destory(*context);
	renderer->Destroy(*context);
	context.reset();
}
