#include <stdexcept>
#include "vulkan-app.h"

void VulkanApp::CreateSyncObjects() {
    VkSemaphoreCreateInfo semaphore_create_info{};
    semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fence_create_info{};
    fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    if (vkCreateSemaphore(device, &semaphore_create_info, nullptr, &image_available_semaphore) != VK_SUCCESS ||
        vkCreateSemaphore(device, &semaphore_create_info, nullptr, &render_finished_semaphore) != VK_SUCCESS ||
        vkCreateFence(device, &fence_create_info, nullptr, &in_flight_fence) != VK_SUCCESS) {
        throw std::runtime_error("failed to create semaphores!");
    }
}

void VulkanApp::OnExecute(MainWindow::OnDrawFrame) {
    vkWaitForFences(device, 1, &in_flight_fence, VK_TRUE, UINT64_MAX);
    vkResetFences(device, 1, &in_flight_fence);

    uint32_t image_index;
    vkAcquireNextImageKHR(device, swapchain_info.swapchain, UINT64_MAX, image_available_semaphore, VK_NULL_HANDLE,
                          &image_index);

    vkResetCommandBuffer(command_buffer, 0);
    RecordCommandBuffer(command_buffer, image_index);


    std::vector<VkSemaphore> wait_semaphores = {
            image_available_semaphore
    };
    std::vector<VkPipelineStageFlags> wait_stages = {
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
    };
    std::vector<VkSemaphore> signal_semaphores = {
            render_finished_semaphore
    };
    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.waitSemaphoreCount = wait_semaphores.size();
    submit_info.pWaitSemaphores = wait_semaphores.data();
    submit_info.pWaitDstStageMask = wait_stages.data();
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &command_buffer;
    submit_info.signalSemaphoreCount = signal_semaphores.size();
    submit_info.pSignalSemaphores = signal_semaphores.data();

    if (vkQueueSubmit(graphics_queue, 1, &submit_info, in_flight_fence) != VK_SUCCESS) {
        throw std::runtime_error("Failed to submit draw command buffer!");
    }

    std::vector<VkSwapchainKHR> swap_chains = {
            swapchain_info.swapchain
    };
    VkPresentInfoKHR present_info{};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.waitSemaphoreCount = signal_semaphores.size();
    present_info.pWaitSemaphores = signal_semaphores.data();
    present_info.swapchainCount = swap_chains.size();
    present_info.pSwapchains = swap_chains.data();
    present_info.pImageIndices = &image_index;
    present_info.pResults = nullptr; // Optional

    vkQueuePresentKHR(present_queue, &present_info);
}
