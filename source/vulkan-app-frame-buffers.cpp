#include <stdexcept>
#include "vulkan-app.h"
#include "vulkan-utils.h"

void VulkanApp::CreateFrameBuffers() {
    auto &image_views = swapchain_info.image_views;
    vk::Extent2D &extent = swapchain_info.extent;
    size_t size_buffers = image_views.size();
    swapchain_framebuffers.resize(size_buffers);
    for (size_t i = 0; i < size_buffers; ++i) {
        vk::FramebufferCreateInfo create_info({}, render_pass, image_views[i], extent.width, extent.height, 1);
        swapchain_framebuffers[i] = device.createFramebuffer(create_info);
    }
}