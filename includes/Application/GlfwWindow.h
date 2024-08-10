#pragma once

#include "config.h"
#include "vulkan/vulkan.hpp"
#include "GLFW/glfw3.h"
#include <vector>
#include "Core/Window.h"
#include <functional>
#include "Engine/InputSystem.h"

class GlfwWindow : public VulkanWindow {
private:
	AppConfig config;
	GLFWwindow* window;
	static uint32_t window_count;
	static std::unordered_map<GLFWwindow*, std::reference_wrapper<InputState>> window_inputs;
	static std::unordered_map<int, InputState::Keyboard> glfw_key_map;
	static std::unordered_map<int, InputState::ActionState> glfw_action_state_map;
	static std::unordered_map<int, InputState::MouseButton> glfw_mouse_button_map;
	static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	static void MousePositionCallback(GLFWwindow* window, double xpos, double ypos);
	static void MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
public:
	GlfwWindow(const AppConfig& config);
	~GlfwWindow();
	bool ShouldClose() const;
	void Update() const;
	inline operator GLFWwindow* () const { return window; }
	inline GLFWwindow* GetRawPtr() const { return window; }
	std::vector<const char*> GetVulkanExtensions() const override;
	std::expected<vk::SurfaceKHR, vk::Result> CreateWindowSurface(const vk::Instance instance, const vk::AllocationCallbacks* allocator) const override;
	vk::Extent2D GetActualExtent() const override;
	inline void RegisterInputState(InputState& input) { window_inputs.insert_or_assign(window, input); }
	inline void UnregisterInputState() { window_inputs.erase(window); }
};
