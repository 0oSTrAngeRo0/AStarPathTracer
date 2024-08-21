#pragma once

#include <memory>
#include <entt/entt.hpp>

class RenderContext;
class Renderer;
class VulkanWindow;
class DeviceContext;
class RendererPipeline;

class RendererApplication {
private:
	std::unique_ptr<RenderContext> render_context;
	std::unique_ptr<Renderer> renderer;
	std::unique_ptr<DeviceContext> context;
	std::unique_ptr<RendererPipeline> pipeline;
	std::unique_ptr<VulkanWindow> window;

	void ResizeWindow();
public:
	RendererApplication(std::unique_ptr<VulkanWindow> window);
	void Update(entt::registry& registry);
	inline RenderContext& GetRenderContext() const { return *render_context; }
	bool IsActive() const;
	~RendererApplication();
};