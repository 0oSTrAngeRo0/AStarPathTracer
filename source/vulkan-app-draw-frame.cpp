#include <stdexcept>
#include "vulkan-app.h"
#include "vulkan-utils.h"

void VulkanApp::CreateSyncObjects() {
    vk::SemaphoreCreateInfo semaphore_create_info;
    vk::FenceCreateInfo fence_create_info(vk::FenceCreateFlagBits::eSignaled);
    image_available_semaphore = device.createSemaphore(semaphore_create_info);
    render_finished_semaphore = device.createSemaphore(semaphore_create_info);
    in_flight_fence = device.createFence(fence_create_info);
}

void VulkanApp::OnExecute(MainWindow::OnDrawFrame) {
    VK_CHECK(device.waitForFences({ in_flight_fence }, vk::True, UINT64_MAX));
    device.resetFences({ in_flight_fence });

    uint32_t image_index = device.acquireNextImageKHR(swapchain_info.swapchain, UINT64_MAX, image_available_semaphore).value;;
    command_buffer.reset();
    RecordCommandBuffer(command_buffer, image_index);


    std::vector<vk::Semaphore> wait_semaphores = {
        image_available_semaphore
    };
    std::vector<vk::PipelineStageFlags> wait_stages = {
        vk::PipelineStageFlagBits::eColorAttachmentOutput
    };
    std::vector<vk::Semaphore> signal_semaphores = {
        render_finished_semaphore
    };
    std::vector<vk::CommandBuffer> command_buffers = {
        command_buffer
    };
    std::vector<vk::SubmitInfo> submit_infos = {
        vk::SubmitInfo(wait_semaphores, wait_stages, command_buffers, signal_semaphores)
    };
    graphics_queue.submit(submit_infos, in_flight_fence);

    std::vector<vk::SwapchainKHR> swap_chains = {
            swapchain_info.swapchain
    };
    vk::PresentInfoKHR present_info(signal_semaphores, swap_chains, image_index);
    VK_CHECK(present_queue.presentKHR(present_info));
}
