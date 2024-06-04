#include <limits>
#include <stdexcept>
#include <functional>
#include "vulkan-app.h"
#include "math-utils.h"
#include "vulkan-utils.h"

VkSurfaceFormatKHR GetSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &formats);

VkPresentModeKHR GetSurfacePresentMode(const std::vector<VkPresentModeKHR> &present_modes);

VkExtent2D GetSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities, const VkExtent2D &preferred_extent);

uint32_t GetMinImageCount(const VkSurfaceCapabilitiesKHR &capabilities);

void CreateImageViews(VulkanApp::SwapchainRuntimeInfo &info, const VkDevice &device);

void VulkanApp::CreateSwapchain() {
    VulkanApp::PhysicalDeviceInfo physical_device_info = physical_device;
    VulkanApp::SwapchainSupportInfo swapchain_support_info = physical_device_info.swapchain_info;
    VkSurfaceCapabilitiesKHR capabilities = swapchain_support_info.capabilities;

    VkSurfaceFormatKHR format = GetSurfaceFormat(swapchain_support_info.formats);
    VkPresentModeKHR present_mode = GetSurfacePresentMode(swapchain_support_info.present_modes);
    VkExtent2D extent = GetSwapExtent(capabilities, actual_extent);
    VkSwapchainCreateInfoKHR create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    create_info.surface = surface;
    create_info.minImageCount = GetMinImageCount(capabilities);
    create_info.imageFormat = format.format;
    create_info.imageColorSpace = format.colorSpace;
    create_info.imageExtent = extent;
    create_info.imageArrayLayers = 1;
    create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    create_info.preTransform = capabilities.currentTransform;
    create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    create_info.presentMode = present_mode;
    create_info.clipped = VK_TRUE;
    create_info.oldSwapchain = VK_NULL_HANDLE;

    VK_CHECK(vkCreateSwapchainKHR(device, &create_info, nullptr, &swapchain_info.swapchain));

    uint32_t runtime_image_count = 0;
    vkGetSwapchainImagesKHR(device, swapchain_info.swapchain, &runtime_image_count, nullptr);
    swapchain_info.images.resize(runtime_image_count);
    vkGetSwapchainImagesKHR(device, swapchain_info.swapchain, &runtime_image_count, swapchain_info.images.data());

    swapchain_info.format = format.format;
    swapchain_info.extent = extent;

    CreateImageViews(swapchain_info, device);
}

VkSurfaceFormatKHR GetSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &formats) {
    for (const VkSurfaceFormatKHR &format: formats) {
        if (format.format == VK_FORMAT_R8G8B8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return format;
        }
    }
    return formats[0];
}

VkPresentModeKHR GetSurfacePresentMode(const std::vector<VkPresentModeKHR> &present_modes) {
    for (const VkPresentModeKHR &present_mode: present_modes) {
        if (present_mode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return present_mode;
        }
    }
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D GetSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities, const VkExtent2D &preferred_extent) {
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    }
    VkExtent2D extent{};
    extent.width = math::Clamp(preferred_extent.width, capabilities.minImageExtent.width,
                              capabilities.maxImageExtent.width);
    extent.height = math::Clamp(preferred_extent.height, capabilities.minImageExtent.height,
                               capabilities.maxImageExtent.height);
    return extent;
}

uint32_t GetMinImageCount(const VkSurfaceCapabilitiesKHR &capabilities) {
    uint32_t image_count = capabilities.minImageCount + 1;
    uint32_t max_image_count = capabilities.maxImageCount;
    if (max_image_count > 0 && max_image_count < image_count) {
        image_count = max_image_count;
    }
    return image_count;
}

void CreateImageViews(VulkanApp::SwapchainRuntimeInfo &info, const VkDevice &device) {
    std::vector<VkImage> &images = info.images;
    std::vector<VkImageView> &image_views = info.image_views;
    size_t image_count = images.size();
    image_views.resize(image_count);
    for (size_t i = 0; i < image_count; ++i) {
        VkComponentMapping components{};
        components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        VkImageSubresourceRange subresource_range{};
        subresource_range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        subresource_range.baseMipLevel = 0;
        subresource_range.levelCount = 1;
        subresource_range.baseArrayLayer = 0;
        subresource_range.layerCount = 1;

        VkImageViewCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        create_info.image = images[i];
        create_info.format = info.format;
        create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        create_info.components = components;
        create_info.subresourceRange = subresource_range;

        VK_CHECK(vkCreateImageView(device, &create_info, nullptr, &image_views[i]));
    }
}
