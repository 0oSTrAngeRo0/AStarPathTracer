#define GLFW_INCLUDE_VULKAN

#include <stdexcept>
#include "Application/GlfwWindow.h"
#include "GLFW/glfw3.h"
#include "config.h"

uint32_t GlfwWindow::window_count = 0;

std::unordered_map<GLFWwindow*, std::reference_wrapper<GlfwWindow>> GlfwWindow::window_inputs = {};

std::unordered_map<int, InputState::Keyboard> GlfwWindow::glfw_key_map = {
	{GLFW_KEY_A, InputState::Keyboard::eA},	{GLFW_KEY_B, InputState::Keyboard::eB},
	{GLFW_KEY_C, InputState::Keyboard::eC},	{GLFW_KEY_D, InputState::Keyboard::eD},
	{GLFW_KEY_E, InputState::Keyboard::eE},	{GLFW_KEY_F, InputState::Keyboard::eF},
	{GLFW_KEY_G, InputState::Keyboard::eG},	{GLFW_KEY_H, InputState::Keyboard::eH},
	{GLFW_KEY_I, InputState::Keyboard::eI},	{GLFW_KEY_J, InputState::Keyboard::eJ},
	{GLFW_KEY_K, InputState::Keyboard::eK},	{GLFW_KEY_L, InputState::Keyboard::eL},
	{GLFW_KEY_M, InputState::Keyboard::eM},	{GLFW_KEY_N, InputState::Keyboard::eN},
	{GLFW_KEY_O, InputState::Keyboard::eO},	{GLFW_KEY_P, InputState::Keyboard::eP},
	{GLFW_KEY_Q, InputState::Keyboard::eQ},	{GLFW_KEY_R, InputState::Keyboard::eR},
	{GLFW_KEY_S, InputState::Keyboard::eS},	{GLFW_KEY_T, InputState::Keyboard::eT},
	{GLFW_KEY_U, InputState::Keyboard::eU}, {GLFW_KEY_V, InputState::Keyboard::eV},
	{GLFW_KEY_W, InputState::Keyboard::eW}, {GLFW_KEY_X, InputState::Keyboard::eX},
	{GLFW_KEY_Y, InputState::Keyboard::eY}, {GLFW_KEY_Z, InputState::Keyboard::eZ}
};

std::unordered_map<int, InputState::MouseButton> GlfwWindow::glfw_mouse_button_map = {
	{GLFW_MOUSE_BUTTON_1, InputState::MouseButton::eLeft},
	{GLFW_MOUSE_BUTTON_2, InputState::MouseButton::eRight},
	{GLFW_MOUSE_BUTTON_3, InputState::MouseButton::eMiddle},
};

std::unordered_map<int, InputState::ActionState> GlfwWindow::glfw_action_state_map = {
	{GLFW_PRESS, InputState::ActionState::ePress},
	{GLFW_REPEAT, InputState::ActionState::eRepeat},
	{GLFW_RELEASE, InputState::ActionState::eRelease},
};

void GlfwWindow::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (!glfw_key_map.contains(key) || !glfw_action_state_map.contains(action)) {
		//std::printf("Invalid Key Action: key:[%d], scancode:[%d], action:[%d], mods:[%d]\n", key, scancode, action, mods);
		return;
	}
	auto input = GetInputState(window);
	if (!input) return;
	InputState::Keyboard mapped_key = glfw_key_map.at(key);
	InputState::ActionState mapped_state = glfw_action_state_map.at(action);
	//std::printf("KeyCallback: key:[%d], state:[%d]\n", mapped_key, mapped_state);
	input.value()->SetKey(mapped_key, mapped_state);
}

void GlfwWindow::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	if (!glfw_mouse_button_map.contains(button) || !glfw_action_state_map.contains(action)) {
		//std::printf("Invalid Key Action: button:[%d], action:[%d], mods:[%d]\n", button, action, mods);
		return;
	}
	auto input = GetInputState(window);
	if (!input) return;
	InputState::MouseButton mapped_button = glfw_mouse_button_map.at(button);
	InputState::ActionState mapped_state = glfw_action_state_map.at(action);
	//std::printf("MouseButtonCallback: button:[%d], state:[%d]\n", mapped_button, mapped_state);
	input.value()->SetMouseButton(mapped_button, mapped_state);
}

void GlfwWindow::MousePositionCallback(GLFWwindow* window, double xpos, double ypos) {
	//std::printf("MousePositionCallback: xpos:[%f], ypos:[%f]\n", xpos, ypos);
	auto input = GetInputState(window);
	if (!input) return;
	input.value()->SetMousePosition(xpos, ypos);
}

void GlfwWindow::MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
	//std::printf("MouseScrollCallback: xoffset:[%f], yoffset:[%f]\n", xoffset, yoffset);
	auto input = GetInputState(window);
	if (!input) return;
	input.value()->SetMouseScroll(xoffset, yoffset);
}

void GlfwWindow::FrameBufferSizeCallback(GLFWwindow* window, int width, int height) {
	if (!window_inputs.contains(window)) return;
	auto& window_wrapper = window_inputs.at(window).get();
	window_wrapper.is_resized = true;
}

std::optional<std::shared_ptr<InputState>> GlfwWindow::GetInputState(GLFWwindow* window) {
	if (!window_inputs.contains(window)) return std::nullopt;
	auto& window_wrapper = window_inputs.at(window).get();
	auto input_ptr = window_wrapper.input;
	if (input_ptr.expired()) return std::nullopt;
	return std::make_optional(input_ptr.lock());
}

void GlfwWindow::PollEvents() {
	for (auto& window : window_inputs) {
		window.second.get().ResetEventState();
	}
	glfwPollEvents();
}

GlfwWindow::GlfwWindow(const AppConfig& config) : name(config.window_title) {
	if (window_count == 0) {
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwSetErrorCallback([](int error_code, const char* message) {
			std::printf("Window Error: [%d], [%s]\n", error_code, message);
		});
		std::printf("Glfw Initialized\n");
	}

	window_count++;
	window = glfwCreateWindow(config.window_width, config.window_height, config.window_title, nullptr, nullptr);

	glfwSetKeyCallback(window, KeyCallback);
	glfwSetMouseButtonCallback(window, MouseButtonCallback);
	glfwSetCursorPosCallback(window, MousePositionCallback);
	glfwSetScrollCallback(window, MouseScrollCallback);
	glfwSetFramebufferSizeCallback(window, FrameBufferSizeCallback);

	window_inputs.insert(std::make_pair(window, std::ref(*this)));

	std::printf("Glfw Window [%s] Created\n", config.window_title);
}

GlfwWindow::~GlfwWindow() {
	glfwDestroyWindow(window);
	window_inputs.erase(window);
	std::printf("Glfw Window [%s] Destroyed\n", name.c_str());
	window_count--;
	if (window_count == 0) {
		glfwTerminate();
		std::printf("Glfw Finalized\n");
	}
}

std::vector<const char*> GlfwWindow::GetVulkanExtensions() const {
	uint32_t count = 0;
	const char** extensions = glfwGetRequiredInstanceExtensions(&count);
	return std::vector<const char*>(extensions, extensions + count);
}

std::expected<vk::SurfaceKHR, vk::Result> GlfwWindow::CreateWindowSurface(const vk::Instance instance, const vk::AllocationCallbacks* allocator) const {
	VkSurfaceKHR surface;
	auto raw_allocator = reinterpret_cast<const VkAllocationCallbacks*>(allocator);
	VkResult result = glfwCreateWindowSurface(instance, window, raw_allocator, &surface);
	if (result == VK_SUCCESS) return vk::SurfaceKHR(surface);
	else return std::unexpected(vk::Result(result));
}

vk::Extent2D GlfwWindow::GetActualExtent() const {
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	return vk::Extent2D(width, height);
}

bool GlfwWindow::IsActive() const {
	return !glfwWindowShouldClose(window);
}
