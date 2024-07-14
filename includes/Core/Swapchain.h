#pragma once

#include <vulkan/vulkan.hpp>

class DeviceContext;

class Swapchain {
private:
    vk::SwapchainKHR swapchain;
    std::vector<vk::Image> images;
    vk::Format format;
    vk::Extent2D extent;
    std::vector<vk::ImageView> image_views;
    uint32_t count;
    uint32_t min_image_count;
    static vk::Extent2D GetSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities, const vk::Extent2D& preferred_extent);
    static uint32_t GetMinImageCount(const VkSurfaceCapabilitiesKHR& capabilities);
    static std::vector<vk::ImageView> CreateImageViews(const DeviceContext& context, std::vector<vk::Image>& images, const vk::Format format);
public:
    Swapchain(const DeviceContext& context);
    void Descroy(const DeviceContext& context);

    inline operator vk::SwapchainKHR() const { return swapchain; }
    inline vk::Extent2D GetExtent() const { return extent; }
    inline vk::Image GetImage(uint32_t index) { return images[index]; }
    inline vk::Format GetFormat() const { return format; }
    inline uint32_t GetMinImageCount() const { return min_image_count; }
    inline uint32_t GetImageCount() const { return count; }
    inline const vk::SwapchainKHR GetSwapchain() const { return swapchain; }
    inline const vk::ImageView GetImageView(uint32_t index) const { return image_views[index]; }
};
