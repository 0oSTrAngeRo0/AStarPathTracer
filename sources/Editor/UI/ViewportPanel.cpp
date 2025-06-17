#include <imgui.h>
#include <imgui_impl_vulkan.h>
#include "Editor/UI/ViewportPanel.h"
#include "Core/Image.h"
#include "Application/Renderer/Renderer.h"

ViewportPanel::ViewportPanel(const DeviceContext& device) {
    renderer = std::make_unique<Renderer>(device);
}

void ViewportPanel::Resize(const DeviceContext& device, std::uint32_t width, std::uint32_t height) {
    if(width == present_image_width && height == present_image_height) return;
    present_image_width = width;
    present_image_height = height;
    if(!present_image) {
        present_image->Destroy(device);
        ImGui_ImplVulkan_RemoveTexture(imgui_descriptor_set);
    }
    present_image = std::make_unique<Image>(device, vk::ImageCreateInfo());
    // imgui_descriptor_set = ImGui_ImplVulkan_AddTexture();
    renderer->ResizeOutput(device, vk::Extent2D(width, height), present_image_format);
}

void ViewportPanel::DrawUi(const DeviceContext& device, vk::CommandBuffer cmd, entt::registry& registry) {
    ImGui::Begin("Viewport");
    ImVec2 size = ImGui::GetContentRegionAvail();
    if(size.x < 100 || size.y < 100) {
        ImGui::Text("Panel is too small to present.");
        ImGui::End();
        return;
    }
    Resize(device, size.x, size.y);
    renderer->PrepareRenderData(device, registry);
    renderer->CmdDraw(cmd);
    renderer->CmdCopyOutputTo(cmd, *present_image);
    ImGui::Image((ImTextureID)(VkDescriptorSet)imgui_descriptor_set, size);
    ImGui::End();
}

void ViewportPanel::Destory(const DeviceContext& device) {
    renderer->Destory(device);
    if(!present_image) {
        present_image->Destroy(device);
        ImGui_ImplVulkan_RemoveTexture(imgui_descriptor_set);
    }
}