#include <limits>
#include <stdexcept>
#include <functional>
#include "vulkan-app.h"
#include "math-utils.h"
#include "vulkan-utils.h"

static vk::Extent2D GetSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities, const vk::Extent2D& preferred_extent) {
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

uint32_t GetMinImageCount(const VkSurfaceCapabilitiesKHR &capabilities);

void CreateImageViews(VulkanApp::SwapchainRuntimeInfo &info, const vk::Device & device);

void VulkanApp::CreateSwapchain(const DeviceContext& context) {
    vk::SurfaceCapabilitiesKHR capabilities = context.GetSurfaceCapabilities();

    vk::SurfaceFormatKHR format = context.GetSurfaceFormat();
    vk::PresentModeKHR present_mode = context.GetSurfacePresentModes();
    vk::Extent2D extent = GetSwapExtent(capabilities, context.GetActuralExtent());
    vk::SwapchainCreateInfoKHR create_info(
        {},
        context.GetSurface(),
        GetMinImageCount(capabilities),
        format.format,
        format.colorSpace,
        extent,
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

    swapchain_info.swapchain = context.GetDevice().createSwapchainKHR(create_info);
    swapchain_info.extent = context.GetActuralExtent();
    swapchain_info.format = format.format;
    swapchain_info.images = context.GetDevice().getSwapchainImagesKHR(swapchain_info.swapchain);
    swapchain_info.count = swapchain_info.images.size();
    CreateImageViews(swapchain_info, context.GetDevice());
}

uint32_t GetMinImageCount(const VkSurfaceCapabilitiesKHR &capabilities) {
    uint32_t image_count = capabilities.minImageCount + 1;
    uint32_t max_image_count = capabilities.maxImageCount;
    if (max_image_count > 0 && max_image_count < image_count) {
        image_count = max_image_count;
    }
    return image_count;
}

void CreateImageViews(VulkanApp::SwapchainRuntimeInfo &info, const vk::Device &device) {
    std::vector<vk::Image> &images = info.images;
    std::vector<vk::ImageView> &image_views = info.image_views;
    size_t image_count = images.size();
    image_views.resize(image_count);

    vk::ComponentMapping components(
        vk::ComponentSwizzle::eIdentity,
        vk::ComponentSwizzle::eIdentity,
        vk::ComponentSwizzle::eIdentity,
        vk::ComponentSwizzle::eIdentity);

    vk::ImageSubresourceRange subresource_range(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);

    for (size_t i = 0; i < image_count; ++i) {
        vk::ImageViewCreateInfo create_info({}, images[i], vk::ImageViewType::e2D, info.format, components, subresource_range);
        image_views[i] = device.createImageView(create_info);
    }
}
