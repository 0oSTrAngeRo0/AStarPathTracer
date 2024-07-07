#include "vulkan-app.h"
#include "acceleration-structure.h"
#include "Core/RenderContext.h"

VulkanApp::VulkanApp(const DeviceContext& context, const RenderContext& render)
{
	CreateSwapchain(context);
	rt_image = std::make_unique<Image>(context, vk::ImageCreateInfo(
		{}, vk::ImageType::e2D, context.GetSurfaceFormat().format,
		vk::Extent3D(swapchain_info.extent, 1), 1, 1, vk::SampleCountFlagBits::e1, vk::ImageTiling::eOptimal,
		vk::ImageUsageFlagBits::eStorage | vk::ImageUsageFlagBits::eTransferSrc));
	rt_image_view = context.GetDevice().createImageView(vk::ImageViewCreateInfo({}, *rt_image,
		vk::ImageViewType::e2D, context.GetSurfaceFormat().format,
		vk::ComponentMapping(vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eG, vk::ComponentSwizzle::eB, vk::ComponentSwizzle::eA),
		vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1)
	));

	UploadDescriptorSet(context, render);
	CreateRayTracingPipelineLayout(context);
	CreateRayTracingPipeline(context);
	shader_binding_table = std::make_unique<RayTracingBindingTable>(context, ray_tracing_pipeline);
	command_pool = context.GetDevice().createCommandPool(vk::CommandPoolCreateInfo(vk::CommandPoolCreateFlagBits::eResetCommandBuffer, context.GetGrpahicsQueueIndex()));
	CreateSyncObjects(context);
}

void VulkanApp::Destroy(const DeviceContext& context) {
	vk::Device device = context.GetDevice();
	device.waitIdle();
	device.destroySemaphore(image_available_semaphore);
	device.destroySemaphore(render_finished_semaphore);
	device.destroyFence(in_flight_fence);
	device.destroyCommandPool(command_pool);
	device.destroyImageView(rt_image_view);
	rt_image->Destroy(context);
	device.destroyDescriptorSetLayout(descriptor_set_layout);
	device.destroyDescriptorPool(descriptor_pool);
	shader_binding_table->Destroy(context);
	device.destroyPipeline(ray_tracing_pipeline);
	device.destroyPipelineLayout(pipeline_layout);
	for (vk::ImageView image_view : swapchain_info.image_views) {
		device.destroyImageView(image_view);
	}
	device.destroySwapchainKHR(swapchain_info.swapchain);
}
