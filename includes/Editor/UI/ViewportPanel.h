#pragma once

#include <memory>
#include <vulkan/vulkan.hpp>
#include <entt/entt.hpp>

class Renderer;
class Image;
class DeviceContext;

class ViewportPanel {
private:
    std::unique_ptr<Renderer> renderer;
    std::unique_ptr<Image> present_image;
    vk::DescriptorSet imgui_descriptor_set;
    vk::Format present_image_format;
    std::uint32_t present_image_width;
    std::uint32_t present_image_height;
public:
    ViewportPanel(const DeviceContext& device);
    void DrawUi(const DeviceContext& device, vk::CommandBuffer cmd, entt::registry& registry);
    void Destory(const DeviceContext& device);
private:
    void Resize(const DeviceContext& device, std::uint32_t width, std::uint32_t height);
};