#pragma once

#include <memory>
#include <entt/entt.hpp>

class GlfwWindow;
class DeviceContext;
class EditorRenderContext;
class EditorUI;
class EditorUIDrawer;

class EditorApplication {
public:
	EditorApplication();
	void Update(entt::registry& registry);
	~EditorApplication();

	inline bool IsActive() const { return is_active; }
private:
	std::unique_ptr<GlfwWindow> window;
	std::unique_ptr<DeviceContext> context;
	std::unique_ptr<EditorRenderContext> render_context;
	std::unique_ptr<EditorUI> ui;
	std::unique_ptr<EditorUIDrawer> ui_drawer;
	bool is_active;
};