#pragma once

#include <memory>

class GlfwWindow;
class DeviceContext;
class EditorRenderContext;
class EditorUI;

class EditorApplication {
public:
	EditorApplication();
	void Update();
	~EditorApplication();
private:
	std::unique_ptr<GlfwWindow> window;
	std::unique_ptr<DeviceContext> context;
	std::unique_ptr<EditorRenderContext> render_context;
	std::unique_ptr<EditorUI> ui;
};