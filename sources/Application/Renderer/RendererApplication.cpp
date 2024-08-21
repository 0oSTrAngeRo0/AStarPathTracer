#include "Core/DeviceContext.h"
#include "config.h"
#include "Core/Window.h"
#include "Application/Renderer/RenderContext.h"
#include "Application/Renderer/Renderer.h"
#include "Application/Renderer/RendererPipeline.h"
#include "Application/Renderer/RendererApplication.h"

RendererApplication::RendererApplication(std::unique_ptr<VulkanWindow> window) : window(std::move(window)) {
	AppConfig config = AppConfig::CreateDefault();
	config.window_title = "Path Tracer Renderer";
	context = std::make_unique<DeviceContext>(*this->window);
	render_context = std::make_unique<RenderContext>(*context);
	renderer = std::make_unique<Renderer>(*context);
	ResizeWindow();
	pipeline = std::make_unique<RendererPipeline>(*context, *render_context, renderer->GetDescriptorPool());
}

void RendererApplication::ResizeWindow() {
	vk::Extent2D extent = window->GetActualExtent();
	renderer->ResizeSwapchain(*context, extent);
	render_context->RecreateOutputImage(*context, extent);
}

void RendererApplication::Update(entt::registry& registry) {
	renderer->WaitForNextFrame(*context);

	if (window->IsResized()) {
		ResizeWindow();
	}

	// prepare resources for rendering
	render_context->Update(*context, registry); 
	pipeline->UpdateDescriptorSet(*context, *render_context);

	// draw frame
	const auto& frame_data = renderer->BeginFrame(*context);
	pipeline->CmdDraw(frame_data.command_buffer, *render_context);
	pipeline->CmdCopyOutputTo(frame_data.command_buffer, *render_context, frame_data.image);
	renderer->EndFrame(*context, frame_data);
}

bool RendererApplication::IsActive() const {
	return window->IsActive();
}

RendererApplication::~RendererApplication() {
	context->GetDevice().waitIdle();
	render_context->Destory(*context);
	pipeline->Destroy(*context);
	renderer->Destroy(*context);
	context.reset();
}
