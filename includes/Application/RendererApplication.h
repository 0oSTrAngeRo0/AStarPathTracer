#pragma once

#include <memory>
#include <entt/entt.hpp>

class RendererPresenter;
class VulkanWindow;
class DeviceContext;
class Renderer;

class RendererApplication {
private:
	std::unique_ptr<Renderer> renderer;
	std::unique_ptr<RendererPresenter> presenter;
	std::unique_ptr<DeviceContext> context;
	std::unique_ptr<VulkanWindow> window;

	void ResizeWindow();
public:
	RendererApplication(std::unique_ptr<VulkanWindow> window);
	void Update(entt::registry& registry);
	bool IsActive() const;
	~RendererApplication();
};