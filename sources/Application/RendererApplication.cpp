#include "Core/DeviceContext.h"
#include "config.h"
#include "Core/Window.h"
#include "Application/Renderer/RenderContext.h"
#include "Application/RendererPresenter.h"
#include "Application/Renderer/RendererPipeline.h"
#include "Application/RendererApplication.h"
#include "Application/RendererDeviceContextCreateConfig.h"
#include "Application/Renderer/Renderer.h"

RendererApplication::RendererApplication(std::unique_ptr<VulkanWindow> window) : window(std::move(window)) {
	AppConfig config = AppConfig::CreateDefault();
	config.window_title = "Path Tracer Renderer";

	RendererDeviceContextCreateInfo context_create_info(*this->window);

	context = std::make_unique<DeviceContext>(context_create_info);
	presenter = std::make_unique<RendererPresenter>(*context, this->window->CreateWindowSurface(context->GetInstance(), nullptr).value());
	presenter->RefreshSurfaceData(*context);
	vk::Extent2D extent = this->window->GetActualExtent();
	presenter->ResizeSwapchain(*context, extent);
	std::printf("Window resized to [%dx%d]\n", extent.width, extent.height);
	renderer = std::make_unique<Renderer>(*context);
	renderer->ResizeOutput(*context, extent, presenter->GetSwapchainFormat());
}

void RendererApplication::ResizeWindow() {
	presenter->RefreshSurfaceData(*context);
	vk::Extent2D extent = window->GetActualExtent();
	presenter->ResizeSwapchain(*context, extent);
	renderer->ResizeOutput(*context, extent, presenter->GetSwapchainFormat());
	std::printf("Window resized to [%dx%d]\n", extent.width, extent.height);
}

void RendererApplication::Update(entt::registry& registry) {
	presenter->WaitForNextFrame(*context);

	if (window->IsResized()) {
		ResizeWindow();
	}

	// prepare resources for rendering
	renderer->PrepareRenderData(*context, registry);

	// draw frame
	const auto& frame_data = presenter->BeginFrame(*context);
	renderer->CmdDraw(frame_data.command_buffer);
	renderer->CmdCopyOutputTo(frame_data.command_buffer, frame_data.image);
	presenter->EndFrame(*context, frame_data);
}

bool RendererApplication::IsActive() const {
	return window->IsActive();
}

RendererApplication::~RendererApplication() {
	context->GetDevice().waitIdle();
	renderer->Destory(*context);
	presenter->Destroy(*context);
	context.reset();
}
