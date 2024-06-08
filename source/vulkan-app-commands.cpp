#include <stdexcept>
#include "vulkan-app.h"
#include "vulkan-utils.h"

void VulkanApp::CreateCommandPool() {
    vk::CommandPoolCreateInfo create_info(vk::CommandPoolCreateFlagBits::eResetCommandBuffer, physical_device.graphics_queue_index);
    command_pool = device.createCommandPool(create_info);
}

void VulkanApp::CreateCommandBuffer() {
    vk::CommandBufferAllocateInfo allocate_info(command_pool, vk::CommandBufferLevel::ePrimary, 1);
    command_buffer = device.allocateCommandBuffers(allocate_info)[0];
}

void VulkanApp::RecordCommandBuffer(vk::CommandBuffer cmd, uint32_t image_index) {
    cmd.begin(vk::CommandBufferBeginInfo());

    std::vector<vk::ClearValue> clear_colors = {
        vk::ClearValue()
    };
    vk::RenderPassBeginInfo render_pass_begin_info(
        render_pass, 
        swapchain_framebuffers[image_index], 
        vk::Rect2D(vk::Offset2D(), swapchain_info.extent),
        clear_colors
    );
    cmd.beginRenderPass(render_pass_begin_info, vk::SubpassContents::eInline);
    cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, graphics_pipeline);

    std::vector<vk::Viewport> viewports = {
        vk::Viewport(0, 0, swapchain_info.extent.width, swapchain_info.extent.height, 0, 1)
    };
    cmd.setViewport(0, viewports);

    std::vector<vk::Rect2D> scissors = {
        vk::Rect2D(vk::Offset2D(), swapchain_info.extent)
    };
    cmd.setScissor(0, scissors);

    mesh.Draw(cmd);

    cmd.draw(3, 1, 0, 0);
    cmd.endRenderPass();
    cmd.end();
}
