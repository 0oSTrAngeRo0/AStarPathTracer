#include <stdexcept>
#include "vulkan-app.h"
#include "vulkan-utils.h"
#include "Engine/Components/Camera.h"
#include "Engine/Components/Transform.h"
#include "Engine/Components/Render.h"
#include "Core/RenderContext.h"

void CmdInsertImageBarrier(const vk::CommandBuffer cmd, vk::ImageMemoryBarrier barrier) {
	barrier.srcQueueFamilyIndex = vk::QueueFamilyIgnored;
	barrier.dstQueueFamilyIndex = vk::QueueFamilyIgnored;
	cmd.pipelineBarrier(vk::PipelineStageFlagBits::eAllCommands, vk::PipelineStageFlagBits::eAllCommands, {}, {}, {}, barrier);
}

void UpdateDescriptorSet(const DeviceContext& context, const RenderContext& render, const vk::DescriptorSet& set) {
	vk::WriteDescriptorSetAccelerationStructureKHR write_tlas(render.tlas);
	vk::DescriptorBufferInfo write_instance_buffer(render.instances.GetBuffer(), 0, vk::WholeSize);
	vk::DescriptorBufferInfo write_constants_buffer(render.constants_buffer, 0, vk::WholeSize);
	std::vector<vk::WriteDescriptorSet> writes = {
		vk::WriteDescriptorSet(set, 0, 0, 1, vk::DescriptorType::eAccelerationStructureKHR, {}, {}, {}, &write_tlas),
		vk::WriteDescriptorSet(set, 5, 0, vk::DescriptorType::eStorageBuffer, {}, write_instance_buffer),
		vk::WriteDescriptorSet(set, 6, 0, vk::DescriptorType::eUniformBuffer, {}, write_constants_buffer),
	};
	context.GetDevice().updateDescriptorSets(writes, {});
}

vk::CommandBuffer VulkanApp::CreateFrameCommandBuffer(const DeviceContext& context, const vk::CommandPool pool, const uint32_t index) {
	vk::Device device = context.GetDevice();
	vk::CommandBufferAllocateInfo allocate_info(pool, vk::CommandBufferLevel::ePrimary, 1);
	vk::CommandBuffer cmd = device.allocateCommandBuffers(allocate_info)[0];

	vk::ImageSubresourceRange subresource(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
	vk::ImageCopy copy_region(
		vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, 0, 0, 1),
		vk::Offset3D(0, 0, 0),
		vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, 0, 0, 1),
		vk::Offset3D(0, 0, 0),
		vk::Extent3D(swapchain_info.extent, 1)
	);

	vk::Image target = swapchain_info.images[index];

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
	return cmd;
}

void VulkanApp::CreateSyncObjects(const DeviceContext& context) {
	vk::SemaphoreCreateInfo semaphore_create_info;
	vk::FenceCreateInfo fence_create_info(vk::FenceCreateFlagBits::eSignaled);
	image_available_semaphore = context.GetDevice().createSemaphore(semaphore_create_info);
	render_finished_semaphore = context.GetDevice().createSemaphore(semaphore_create_info);
	in_flight_fence = context.GetDevice().createFence(fence_create_info);
}

void VulkanApp::Draw(const DeviceContext& context, const RenderContext& render) {
	VK_CHECK(context.GetDevice().waitForFences({ in_flight_fence }, vk::True, UINT64_MAX));
	context.GetDevice().resetFences({ in_flight_fence });

	UpdateDescriptorSet(context, render, descriptor_set);
	uint32_t image_index = context.GetDevice().acquireNextImageKHR(swapchain_info.swapchain, UINT64_MAX, image_available_semaphore).value;
	vk::CommandBuffer cmd = CreateFrameCommandBuffer(context, command_pool, image_index);

	vk::PipelineStageFlags stage_mask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
	vk::SubmitInfo submit_info(
		image_available_semaphore,
		stage_mask,
		cmd,
		render_finished_semaphore
	);
	context.GetGraphicsQueue().submit(submit_info, in_flight_fence);
	context.GetGraphicsQueue().waitIdle(); // 等待当前帧执行完毕， TODO: 优化之
	vk::PresentInfoKHR present_info(render_finished_semaphore, swapchain_info.swapchain, image_index);
	VK_CHECK(context.GetPresentQueue().presentKHR(present_info));
}
