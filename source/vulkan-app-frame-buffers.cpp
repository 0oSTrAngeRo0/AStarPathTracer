#include <stdexcept>
#include "vulkan-app.h"
#include "vulkan-utils.h"

void VulkanApp::CreateFrameBuffers() {
    std::vector<VkImageView> &image_views = swapchain_info.image_views;
    VkExtent2D &extent = swapchain_info.extent;
    size_t size_buffers = image_views.size();
    swapchain_framebuffers.resize(size_buffers);
    for (size_t i = 0; i < size_buffers; ++i) {
        VkFramebufferCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        create_info.renderPass = render_pass;
        create_info.attachmentCount = 1;
        create_info.pAttachments = &image_views[i];
        create_info.width = extent.width;
        create_info.height = extent.height;
        create_info.layers = 1;

        VK_CHECK(vkCreateFramebuffer(device, &create_info, nullptr, &swapchain_framebuffers[i]));
    }
}