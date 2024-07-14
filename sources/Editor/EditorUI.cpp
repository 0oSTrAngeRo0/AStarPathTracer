#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

#include "Editor/EditorUI.h"

#include "Core/DeviceContext.h"
#include "main-window.h"
#include "Core/Swapchain.h"
#include "Editor/EditorRenderContext.h"

#include <stdio.h>          // printf, fprintf

void EditorUI::CheckVkResult(VkResult err) {
	if (err == 0) return;
	fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
}

EditorUI::EditorUI(const DeviceContext& context, const EditorRenderContext& render_context, const GlfwWindow& window) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
	//io.ConfigViewportsNoAutoMerge = true;
	//io.ConfigViewportsNoTaskBarIcon = true;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForVulkan(window.GetRawPtr(), true);
	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.Instance = context.GetInstance();
	init_info.PhysicalDevice = context.GetGpu();
	init_info.Device = context.GetDevice();
	init_info.QueueFamily = context.GetGrpahicsQueueIndex();
	init_info.Queue = context.GetGraphicsQueue();
	init_info.PipelineCache = VK_NULL_HANDLE;
	init_info.DescriptorPool = render_context.GetDescriptorPool();
	init_info.RenderPass = render_context.GetRenderPass();
	init_info.Subpass = 0;
	init_info.MinImageCount = render_context.GetSwapchain().GetMinImageCount();
	init_info.ImageCount = render_context.GetSwapchain().GetImageCount();
	init_info.MSAASamples = (VkSampleCountFlagBits)vk::SampleCountFlagBits::e1;
	init_info.Allocator = nullptr;
	init_info.CheckVkResultFn = CheckVkResult;
	ImGui_ImplVulkan_Init(&init_info);
}

void EditorUI::UpdateBeginFrame() {
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::ShowDemoWindow();
	ImGui::Render();
}

void EditorUI::CmdDraw(vk::CommandBuffer cmd) {
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);
}

//void EditorRenderContext::CmdDrawUI(EditorFrameContext& frame, EditorUI& ui) {
//	vk::ClearValue clear_value = vk::ClearValue(vk::ClearColorValue());
//	frame.command_buffer.beginRenderPass(vk::RenderPassBeginInfo(
//		render_pass, frame.framebuffer,
//		vk::Rect2D(vk::Offset2D(0, 0), swapchain->GetExtent()), clear_value),
//		vk::SubpassContents::eInline);
//	ui.CmdDraw(frame.command_buffer);
//	frame.command_buffer.endRenderPass();
//}

void EditorUI::UpdateEndFrame() {
	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
}

EditorUI::~EditorUI() {
	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}
