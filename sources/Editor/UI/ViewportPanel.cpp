#include <imgui.h>
#include <imgui_impl_vulkan.h>
#include "Editor/UI/ViewportPanel.h"
#include "Core/Image.h"
#include "Application/Renderer/Renderer.h"
#include "Core/DeviceContext.h"
#include "Application/Renderer/CommandUtilities.h"

void ViewportPanel::Resize(const DeviceContext& device, std::uint32_t width, std::uint32_t height) {
    if(width == present_image_width && height == present_image_height) return;
    present_image_width = width;
    present_image_height = height;
    present_image_format = vk::Format::eR8G8B8A8Unorm;
    DestroyPresentImage(device);
    present_image = Image(device, vk::ImageCreateInfo(
        {},
        vk::ImageType::e2D,
        present_image_format,
        vk::Extent3D(width, height, 1),
        1,
        1,
        vk::SampleCountFlagBits::e1,
        vk::ImageTiling::eOptimal,
        vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled
    ));
    present_image_view = device.GetDevice().createImageView(vk::ImageViewCreateInfo(
        {}, 
        present_image, 
        vk::ImageViewType::e2D, 
        present_image_format,
        vk::ComponentMapping(vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eG, vk::ComponentSwizzle::eB, vk::ComponentSwizzle::eA),
        vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1)
    ));
    present_image_sampler = device.GetDevice().createSampler(vk::SamplerCreateInfo(
        {}, vk::Filter::eLinear, vk::Filter::eLinear, vk::SamplerMipmapMode::eNearest,
        vk::SamplerAddressMode::eClampToEdge, vk::SamplerAddressMode::eClampToEdge, vk::SamplerAddressMode::eClampToEdge,
        {}, vk::False, {}, vk::False, vk::CompareOp::eAlways, {}, {}, vk::BorderColor::eFloatOpaqueBlack, vk::False
    ));
    imgui_descriptor_set = ImGui_ImplVulkan_AddTexture(present_image_sampler, present_image_view, (VkImageLayout)vk::ImageLayout::eShaderReadOnlyOptimal);
    renderer.ResizeOutput(device, vk::Extent2D(width, height), present_image_format);
}

void ViewportPanel::DrawUi(const DeviceContext& device, vk::CommandBuffer cmd, entt::registry& registry) {
    ImGui::Begin("Viewport");
    ImVec2 size = ImGui::GetContentRegionAvail();
    Resize(device, size.x, size.y);
    renderer.PrepareRenderData(device, registry);
    renderer.CmdDraw(cmd);
    renderer.CmdCopyOutputTo(cmd, present_image);
    CommandUtilities::CmdInsertImageBarrier(cmd, vk::ImageMemoryBarrier(
        vk::AccessFlagBits::eNone, vk::AccessFlagBits::eShaderRead,
        vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal,
        {}, {}, present_image, vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1)
    ));
    ImGui::Image((ImTextureID)(VkDescriptorSet)imgui_descriptor_set, size);
    ImGui::End();
}

void ViewportPanel::DestroyPresentImage(const DeviceContext& device) {
    if(!(vk::Image)present_image) return;
    ImGui_ImplVulkan_RemoveTexture(imgui_descriptor_set);
    device.GetDevice().destroySampler(present_image_sampler);
    device.GetDevice().destroyImageView(present_image_view);
    present_image.Destroy(device);
}

void ViewportPanel::Destory(const DeviceContext& device) {
    DestroyPresentImage(device);
    renderer.Destory(device);
}