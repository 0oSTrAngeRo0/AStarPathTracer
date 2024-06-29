#include "vulkan-app.h"
#include "acceleration-structure.h"
#include "Core/Scene.h"

VulkanApp::VulkanApp(const AppConfig& config, MainWindow* main_window)
{
	this->config = config;
	context = std::make_unique<DeviceContext>(*main_window);
	CreateSwapchain();

	scene = std::make_unique<Scene>(*context);

	rt_image = std::make_unique<Image>(*context, vk::ImageCreateInfo(
		{}, vk::ImageType::e2D, context->GetSurfaceFormat().format,
		vk::Extent3D(swapchain_info.extent, 1), 1, 1, vk::SampleCountFlagBits::e1, vk::ImageTiling::eOptimal,
		vk::ImageUsageFlagBits::eStorage | vk::ImageUsageFlagBits::eTransferSrc));
	rt_image_view = context->GetDevice().createImageView(vk::ImageViewCreateInfo({}, *rt_image,
		vk::ImageViewType::e2D, context->GetSurfaceFormat().format,
		vk::ComponentMapping(vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eG, vk::ComponentSwizzle::eB, vk::ComponentSwizzle::eA),
		vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1)
	));

	UploadDescriptorSet();
	CreateRayTracingPipelineLayout();
	CreateRayTracingPipeline();
	shader_binding_table = std::make_unique<RayTracingBindingTable>(*context, ray_tracing_pipeline);
	CreateCommandBuffers();
	CreateSyncObjects();
}

void VulkanApp::OnExecute(MainWindow::OnDestroyed) {
	vk::Device device = context->GetDevice();
	device.waitIdle();
	device.destroySemaphore(image_available_semaphore);
	device.destroySemaphore(render_finished_semaphore);
	device.destroyFence(in_flight_fence);
	device.destroyCommandPool(command_pool);
	device.destroyImageView(rt_image_view);
	rt_image->Destroy(*context);
	scene->Destory(*context);
	device.destroyDescriptorSetLayout(descriptor_set_layout);
	device.destroyDescriptorPool(descriptor_pool);
	shader_binding_table->Destroy(*context);
	device.destroyPipeline(ray_tracing_pipeline);
	device.destroyPipelineLayout(pipeline_layout);
	for (vk::ImageView image_view : swapchain_info.image_views) {
		device.destroyImageView(image_view);
	}
	device.destroySwapchainKHR(swapchain_info.swapchain);
	context.reset();
}
