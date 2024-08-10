#pragma once

#include <memory>
#include <entt/entt.hpp>

class RenderContext;
class Renderer;
class VulkanWindow;
class DeviceContext;

class RendererApplication {
private:
	std::unique_ptr<RenderContext> render_context;
	std::unique_ptr<Renderer> renderer;
	std::unique_ptr<DeviceContext> context;
public:
	RendererApplication(const VulkanWindow& window);
	void Update(entt::registry& registry);
	inline RenderContext& GetRenderContext() const { return *render_context; }
	~RendererApplication();
};