#pragma once

#include <memory>
#include <entt/entt.hpp>

class RenderContext;
class Renderer;
class GlfwWindow;
class DeviceContext;

class RendererApplication {
private:
	std::unique_ptr<RenderContext> render_context;
	std::unique_ptr<Renderer> renderer;
	std::unique_ptr<DeviceContext> context;
	std::unique_ptr<GlfwWindow> window;
	bool is_active;
public:
	RendererApplication();
	void Update(entt::registry& registry);
	inline bool IsActive() const { return is_active; }
	inline RenderContext& GetRenderContext() const { return *render_context; }
	~RendererApplication();
};