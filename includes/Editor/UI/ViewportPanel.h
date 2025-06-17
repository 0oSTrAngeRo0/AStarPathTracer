#pragma once

#include <memory>
#include <vulkan/vulkan.hpp>
#include <entt/entt.hpp>
#include "Core/Image.h"
#include "Application/Renderer/Renderer.h"

class DeviceContext;

class ViewportPanel {
private:
    Renderer renderer;
    Image present_image;
    vk::DescriptorSet imgui_descriptor_set;
    vk::Format present_image_format;
    vk::ImageView present_image_view;
    vk::Sampler present_image_sampler;
    std::uint32_t present_image_width;
    std::uint32_t present_image_height;
public:
    ViewportPanel(const DeviceContext& device) : renderer(device) {}
    void DrawUi(const DeviceContext& device, vk::CommandBuffer cmd, entt::registry& registry);
    void Destory(const DeviceContext& device);
private:
    void Resize(const DeviceContext& device, std::uint32_t width, std::uint32_t height);
    void DestroyPresentImage(const DeviceContext& device);
};