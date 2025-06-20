#include "Core/DeviceContext.h"
#include "Application/Renderer/RenderContext.h"
#include <stdexcept>
#include "Core/VulkanUtils.h"
#include "Engine/Components/Camera.h"
#include "Engine/Components/Transform.h"
#include "Engine/Components/Render.h"
#include "Application/Renderer/RenderContext.h"
#include "Application/RendererPresenter.h"
#include "file-system.h"
#include "Core/VulkanUtils.h"
#include "math-utils.h"
#include "acceleration-structure.h"
#include "Application/Renderer/RenderContext.h"
#include "Engine/Resources/ResourcesManager.h"
#include "Engine/Resources/ResourceData.h"
#include "Engine/HostShaderManager.h"
#include "Application/Renderer/RendererPipeline.h"
#include "Core/Swapchain.h"
#include "Core/Surface.h"

RendererPresenter::RendererPresenter(const DeviceContext& context, vk::SurfaceKHR surface) {
	command_pool = context.GetDevice().createCommandPool(vk::CommandPoolCreateInfo(vk::CommandPoolCreateFlagBits::eResetCommandBuffer, context.GetGrpahicsQueueIndex()));
	CreateSyncObjects(context);
	this->surface = std::make_unique<Surface>(context, surface);
}

void RendererPresenter::CreateSyncObjects(const DeviceContext& context) {
	vk::SemaphoreCreateInfo semaphore_create_info;
	vk::FenceCreateInfo fence_create_info(vk::FenceCreateFlagBits::eSignaled);
	image_available_semaphore = context.GetDevice().createSemaphore(semaphore_create_info);
	render_finished_semaphore = context.GetDevice().createSemaphore(semaphore_create_info);
	in_flight_fence = context.GetDevice().createFence(fence_create_info);
}

void RendererPresenter::WaitForNextFrame(const DeviceContext& context) {
	VK_CHECK(context.GetDevice().waitForFences({ in_flight_fence }, vk::True, UINT64_MAX));
}

const RendererPresenter::FrameData RendererPresenter::BeginFrame(const DeviceContext& context) {
	auto next_image_result = context.GetDevice().acquireNextImageKHR(*swapchain, UINT64_MAX, image_available_semaphore);
	VK_CHECK(next_image_result.result);
	
	context.GetDevice().resetFences({ in_flight_fence });

	uint32_t image_index = next_image_result.value;

	vk::CommandBufferAllocateInfo allocate_info(command_pool, vk::CommandBufferLevel::ePrimary, 1);
	vk::CommandBuffer cmd = context.GetDevice().allocateCommandBuffers(allocate_info)[0];
	cmd.begin(vk::CommandBufferBeginInfo());
	return RendererPresenter::FrameData(cmd, image_index, swapchain->GetImage(image_index));
}

void RendererPresenter::EndFrame(const DeviceContext& context, const RendererPresenter::FrameData& frame_data) {
	frame_data.command_buffer.end();

	vk::PipelineStageFlags stage_mask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
	vk::SubmitInfo submit_info(
		image_available_semaphore,
		stage_mask,
		frame_data.command_buffer,
		render_finished_semaphore
	);
	context.GetGraphicsQueue().submit(submit_info, in_flight_fence);
	context.GetGraphicsQueue().waitIdle(); // 等待当前帧执行完毕， TODO: 优化之
	context.GetDevice().freeCommandBuffers(command_pool, frame_data.command_buffer);
	std::vector<vk::SwapchainKHR> swapchains = { *swapchain };
	vk::PresentInfoKHR present_info(render_finished_semaphore, swapchains, frame_data.image_index);
	VK_CHECK(context.GetPresentQueue().presentKHR(present_info));
}

void RendererPresenter::ResizeSwapchain(const DeviceContext& context, const vk::Extent2D extent) {
	if (swapchain) {
		swapchain->Destroy(context);
	}
	swapchain = std::make_unique<Swapchain>(context, *surface, extent);
}

void RendererPresenter::RefreshSurfaceData(const DeviceContext& context) {
	return surface->ReacquireProperties(context);
}

const vk::Format RendererPresenter::GetSwapchainFormat() const {
	return surface->GetSurfaceFormat().format;
}

void RendererPresenter::Destroy(const DeviceContext& context) {
	vk::Device device = context.GetDevice();
	device.destroySemaphore(image_available_semaphore);
	device.destroySemaphore(render_finished_semaphore);
	device.destroyFence(in_flight_fence);
	device.destroyCommandPool(command_pool);
	swapchain->Destroy(context);
	surface->Destroy(context);
}

RendererPresenter::~RendererPresenter() = default;
