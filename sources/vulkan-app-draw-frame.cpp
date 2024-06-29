#include <stdexcept>
#include "vulkan-app.h"
#include "vulkan-utils.h"

void CmdInsertImageBarrier(const vk::CommandBuffer cmd, vk::ImageMemoryBarrier barrier) {
	barrier.srcQueueFamilyIndex = vk::QueueFamilyIgnored;
	barrier.dstQueueFamilyIndex = vk::QueueFamilyIgnored;
	cmd.pipelineBarrier(vk::PipelineStageFlagBits::eAllCommands, vk::PipelineStageFlagBits::eAllCommands, {}, {}, {}, barrier);
}

void VulkanApp::CreateCommandBuffers() {
	vk::Device device = context->GetDevice();
	vk::CommandPoolCreateInfo create_info(vk::CommandPoolCreateFlagBits::eResetCommandBuffer, context->GetGrpahicsQueueIndex());
	command_pool = device.createCommandPool(create_info);

	vk::CommandBufferAllocateInfo allocate_info(command_pool, vk::CommandBufferLevel::ePrimary, swapchain_info.count);
	command_buffers = device.allocateCommandBuffers(allocate_info);

	vk::ImageSubresourceRange subresource(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
	vk::ImageCopy copy_region(
		vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, 0, 0, 1),
		vk::Offset3D(0, 0, 0),
		vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, 0, 0, 1),
		vk::Offset3D(0, 0, 0),
		vk::Extent3D(swapchain_info.extent, 1)
	);

	for (uint32_t i = 0, end = swapchain_info.count; i < end; i++)
	{
		vk::CommandBuffer cmd = command_buffers[i];
		vk::Image target = swapchain_info.images[i];

		cmd.begin(vk::CommandBufferBeginInfo());
		CmdInsertImageBarrier(cmd, vk::ImageMemoryBarrier(
			vk::AccessFlagBits::eNone, vk::AccessFlagBits::eShaderWrite,
			vk::ImageLayout::eUndefined, vk::ImageLayout::eGeneral,
			{}, {}, *rt_image, subresource));
		cmd.bindPipeline(vk::PipelineBindPoint::eRayTracingKHR, ray_tracing_pipeline);
		cmd.bindDescriptorSets(vk::PipelineBindPoint::eRayTracingKHR, pipeline_layout, 0, vk::ArrayProxy(descriptor_set), {});
		cmd.traceRaysKHR(
			shader_binding_table->rgen,
			shader_binding_table->miss,
			shader_binding_table->hit,
			shader_binding_table->call,
			swapchain_info.extent.width,
			swapchain_info.extent.height,
			1
		);
		CmdInsertImageBarrier(cmd, vk::ImageMemoryBarrier(
			vk::AccessFlagBits::eNone, vk::AccessFlagBits::eShaderWrite,
			vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal,
			{}, {}, target, subresource));
		CmdInsertImageBarrier(cmd, vk::ImageMemoryBarrier(
			vk::AccessFlagBits::eShaderWrite, vk::AccessFlagBits::eShaderRead,
			vk::ImageLayout::eGeneral, vk::ImageLayout::eTransferSrcOptimal,
			{}, {}, *rt_image, subresource));
		cmd.copyImage(*rt_image, vk::ImageLayout::eTransferSrcOptimal, target, vk::ImageLayout::eTransferDstOptimal, copy_region);
		CmdInsertImageBarrier(cmd, vk::ImageMemoryBarrier(
			vk::AccessFlagBits::eNone, vk::AccessFlagBits::eShaderWrite,
			vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::ePresentSrcKHR,
			{}, {}, target, subresource));
		cmd.end();
	}
}

void VulkanApp::CreateSyncObjects() {
	vk::SemaphoreCreateInfo semaphore_create_info;
	vk::FenceCreateInfo fence_create_info(vk::FenceCreateFlagBits::eSignaled);
	image_available_semaphore = context->GetDevice().createSemaphore(semaphore_create_info);
	render_finished_semaphore = context->GetDevice().createSemaphore(semaphore_create_info);
	in_flight_fence = context->GetDevice().createFence(fence_create_info);
}

void VulkanApp::OnExecute(MainWindow::OnDrawFrame) {
	VK_CHECK(context->GetDevice().waitForFences({ in_flight_fence }, vk::True, UINT64_MAX));
	context->GetDevice().resetFences({ in_flight_fence });

	uint32_t image_index = context->GetDevice().acquireNextImageKHR(swapchain_info.swapchain, UINT64_MAX, image_available_semaphore).value;;

	vk::PipelineStageFlags stage_mask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
	vk::SubmitInfo submit_info(
		image_available_semaphore,
		stage_mask,
		command_buffers[image_index],
		render_finished_semaphore
	);
	context->GetGraphicsQueue().submit(submit_info, in_flight_fence);

	vk::PresentInfoKHR present_info(render_finished_semaphore, swapchain_info.swapchain, image_index);
	VK_CHECK(context->GetPresentQueue().presentKHR(present_info));
}
