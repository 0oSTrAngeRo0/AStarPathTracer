#pragma once

#include <vulkan/vulkan.hpp>
#include <vector>
#include "Core/Window.h"
#include "Engine/InputSystem.h"

class GLFWwindow;
class AppConfig;

class GlfwWindow : public VulkanWindow {
public:
	GlfwWindow(const AppConfig& config);
	~GlfwWindow();
	inline operator GLFWwindow* () const { return window; }
	inline GLFWwindow* GetRawPtr() const { return window; }
	std::vector<std::string> GetVulkanExtensions() const override;
	std::expected<vk::SurfaceKHR, vk::Result> CreateWindowSurface(const vk::Instance instance, const vk::AllocationCallbacks* allocator) const override;
	vk::Extent2D GetActualExtent() const override;
	bool IsActive() const override;
	bool IsResized() const override { return is_resized; }
	inline void RegisterInputState(std::shared_ptr<InputState> input) { this->input = input; }

	static void PollEvents();

private:
	std::string name;
	GLFWwindow* window;
	std::weak_ptr<InputState> input;
	bool is_resized;

	inline void ResetEventState() {
		is_resized = false;
	}

	static uint32_t window_count;
	static std::unordered_map<GLFWwindow*, std::reference_wrapper<GlfwWindow>> window_inputs;
	static std::unordered_map<int, InputState::Keyboard> glfw_key_map;
	static std::unordered_map<int, InputState::ActionState> glfw_action_state_map;
	static std::unordered_map<int, InputState::MouseButton> glfw_mouse_button_map;
	static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	static void MousePositionCallback(GLFWwindow* window, double xpos, double ypos);
	static void MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
	static void FrameBufferSizeCallback(GLFWwindow* window, int width, int height);
	static std::optional<std::shared_ptr<InputState>> GetInputState(GLFWwindow* window);
};
