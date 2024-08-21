#include "Core/DeviceContext.h"
#include "Core/Swapchain.h"
#include "math-utils.h"
#include "vulkan/vulkan_raii.hpp"

vk::Extent2D Swapchain::GetSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities, const vk::Extent2D& preferred_extent) {
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    }
    vk::Extent2D extent{};
    extent.width = math::Clamp(preferred_extent.width, capabilities.minImageExtent.width,
        capabilities.maxImageExtent.width);
    extent.height = math::Clamp(preferred_extent.height, capabilities.minImageExtent.height,
        capabilities.maxImageExtent.height);
    return extent;
}

uint32_t Swapchain::GetMinImageCount(const VkSurfaceCapabilitiesKHR& capabilities) {
    uint32_t image_count = capabilities.minImageCount + 1;
    uint32_t max_image_count = capabilities.maxImageCount;
    if (max_image_count > 0 && max_image_count < image_count) {
        image_count = max_image_count;
    }
    return image_count;
}

std::vector<vk::ImageView> Swapchain::CreateImageViews(const DeviceContext& context, std::vector<vk::Image>& images, const vk::Format format) {
    size_t image_count = images.size();
    std::vector<vk::ImageView> image_views(image_count);

    vk::ComponentMapping components(
        vk::ComponentSwizzle::eIdentity,
        vk::ComponentSwizzle::eIdentity,
        vk::ComponentSwizzle::eIdentity,
        vk::ComponentSwizzle::eIdentity);

    vk::ImageSubresourceRange subresource_range(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);

    for (size_t i = 0; i < image_count; ++i) {
        vk::ImageViewCreateInfo create_info({}, images[i], vk::ImageViewType::e2D, format, components, subresource_range);
        image_views[i] = context.GetDevice().createImageView(create_info);
    }
    return image_views;
}

Swapchain::Swapchain(const DeviceContext& context) {
    vk::Device device = context.GetDevice();
    vk::SurfaceCapabilitiesKHR capabilities = context.GetSurfaceCapabilities();

    vk::SurfaceFormatKHR surface_format = context.GetSurfaceFormat();
    vk::PresentModeKHR present_mode = context.GetSurfacePresentModes();
    vk::Extent2D surface_extent = GetSwapExtent(capabilities, context.GetActuralExtent());
    min_image_count = GetMinImageCount(capabilities);
    vk::SwapchainCreateInfoKHR create_info(
        {},
        context.GetSurface(),
        min_image_count,
        surface_format.format,
        surface_format.colorSpace,
        surface_extent,
        1,
        vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst,
        vk::SharingMode::eExclusive,
        {},
        capabilities.currentTransform,
        vk::CompositeAlphaFlagBitsKHR::eOpaque,
        present_mode,
        vk::True,
        {},
        {});

    swapchain = device.createSwapchainKHR(create_info);
    extent = context.GetActuralExtent();
    format = surface_format.format;
    images = device.getSwapchainImagesKHR(swapchain);
    count = images.size();
    image_views = CreateImageViews(context, images, format);
}

void Swapchain::Destroy(const DeviceContext& context) {
    vk::Device device = context.GetDevice();
    for (vk::ImageView image_view : image_views) {
        device.destroyImageView(image_view);
    }
    device.destroySwapchainKHR(swapchain);
}
