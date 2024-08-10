#define GLFW_INCLUDE_VULKAN

#include <stdexcept>
#include "Application/GlfwWindow.h"
#include "GLFW/glfw3.h"
#include "event-registry.h"

uint32_t GlfwWindow::window_count = 0;

std::unordered_map<GLFWwindow*, std::reference_wrapper<InputState>> GlfwWindow::window_inputs = {};

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
	if (!window_inputs.contains(window)) return;
	InputState& input = window_inputs.at(window);
	if (!glfw_key_map.contains(key) || !glfw_action_state_map.contains(action)) {
		//std::printf("Invalid Key Action: key:[%d], scancode:[%d], action:[%d], mods:[%d]\n", key, scancode, action, mods);
		return;
	}
	InputState::Keyboard mapped_key = glfw_key_map.at(key);
	InputState::ActionState mapped_state = glfw_action_state_map.at(action);
	//std::printf("KeyCallback: key:[%d], state:[%d]\n", mapped_key, mapped_state);
	input.SetKey(mapped_key, mapped_state);
}

void GlfwWindow::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	if (!window_inputs.contains(window)) return;
	InputState& input = window_inputs.at(window);
	if (!glfw_mouse_button_map.contains(button) || !glfw_action_state_map.contains(action)) {
		//std::printf("Invalid Key Action: button:[%d], action:[%d], mods:[%d]\n", button, action, mods);
		return;
	}
	InputState::MouseButton mapped_button = glfw_mouse_button_map.at(button);
	InputState::ActionState mapped_state = glfw_action_state_map.at(action);
	//std::printf("MouseButtonCallback: button:[%d], state:[%d]\n", mapped_button, mapped_state);
	input.SetMouseButton(mapped_button, mapped_state);
}

void GlfwWindow::MousePositionCallback(GLFWwindow* window, double xpos, double ypos) {
	if (!window_inputs.contains(window)) return;
	InputState& input = window_inputs.at(window);
	//std::printf("MousePositionCallback: xpos:[%f], ypos:[%f]\n", xpos, ypos);
	input.SetMousePosition(xpos, ypos);
}

void GlfwWindow::MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
	if (!window_inputs.contains(window)) return;
	InputState& input = window_inputs.at(window);
	//std::printf("MouseScrollCallback: xoffset:[%f], yoffset:[%f]\n", xoffset, yoffset);
	input.SetMouseScroll(xoffset, yoffset);
}

GlfwWindow::GlfwWindow(const AppConfig& config) : config(config) {
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
	std::printf("Glfw Window [%s] Created\n", config.window_title);
}

GlfwWindow::~GlfwWindow() {
	glfwDestroyWindow(window);
	std::printf("Glfw Window [%s] Destroyed\n", config.window_title);
	window_count--;
	if (window_count == 0) {
		glfwTerminate();
		std::printf("Glfw Finalized\n");
	}
}

bool GlfwWindow::ShouldClose() const {
	return glfwWindowShouldClose(window);
}

void GlfwWindow::Update() const {
	glfwPollEvents();
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
