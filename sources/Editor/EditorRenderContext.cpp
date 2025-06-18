#include "Editor/EditorRenderContext.h"

#include "Core/DeviceContext.h"
#include "Core/Swapchain.h"
#include "Core/Surface.h"
#include "Core/VulkanUtils.h"
#include "Editor/EditorUI.h"

EditorFrameContext::EditorFrameContext(
	const DeviceContext& context,
	const vk::CommandBuffer cmd,
	const vk::Framebuffer framebuffer
) : command_buffer(cmd), framebuffer(framebuffer) {
	vk::Device device = context.GetDevice();
	image_available_semaphore = device.createSemaphore(vk::SemaphoreCreateInfo());
	render_finished_semaphore = device.createSemaphore(vk::SemaphoreCreateInfo());
	in_flight_fence = device.createFence(vk::FenceCreateInfo(vk::FenceCreateFlagBits::eSignaled));
}

void EditorFrameContext::Destroy(const DeviceContext& context) {
	vk::Device device = context.GetDevice();
	device.destroySemaphore(image_available_semaphore);
	device.destroySemaphore(render_finished_semaphore);
	device.destroyFence(in_flight_fence);
	device.destroyFramebuffer(framebuffer);
}

std::vector<EditorFrameContext> EditorFrameContext::CreateFrameContext(
	const DeviceContext& context,
	const vk::CommandPool pool,
	const Swapchain& swapchain,
	const vk::RenderPass& pass
) {
	vk::Device device = context.GetDevice();
	uint32_t count = swapchain.GetImageCount();
	auto cmds = context.GetDevice().allocateCommandBuffers(vk::CommandBufferAllocateInfo(pool, vk::CommandBufferLevel::ePrimary, count));
	std::vector<EditorFrameContext> frames;
	vk::Extent2D extent = swapchain.GetExtent();
	for (uint32_t i = 0; i < count; i++) {
		vk::ImageView attachments[] = { swapchain.GetImageView(i) };
		vk::Framebuffer framebuffer = device.createFramebuffer(vk::FramebufferCreateInfo({}, pass, attachments, extent.width, extent.height, 1));
		frames.emplace_back(EditorFrameContext(context, cmds[i], framebuffer));
	}
	return frames;
}

vk::RenderPass EditorRenderContext::CreateRenderPass(const DeviceContext& context, const vk::Format format) {
	vk::AttachmentDescription attachment(
		{},
		format,
		vk::SampleCountFlagBits::e1,
		vk::AttachmentLoadOp::eClear,
		vk::AttachmentStoreOp::eStore,
		vk::AttachmentLoadOp::eDontCare,
		vk::AttachmentStoreOp::eDontCare,
		vk::ImageLayout::eUndefined,
		vk::ImageLayout::ePresentSrcKHR
	);

	std::vector<vk::AttachmentReference> references = { vk::AttachmentReference(0, vk::ImageLayout::eColorAttachmentOptimal) };

	vk::SubpassDescription subpass(
		{},
		vk::PipelineBindPoint::eGraphics,
		{},
		references
	);
	vk::SubpassDependency dependency(
		vk::SubpassExternal,
		0,
		vk::PipelineStageFlagBits::eColorAttachmentOutput,
		vk::PipelineStageFlagBits::eColorAttachmentOutput,
		{},
		vk::AccessFlagBits::eColorAttachmentWrite
	);
	vk::RenderPassCreateInfo create_info({}, attachment, subpass, dependency);
	return context.GetDevice().createRenderPass(create_info);
}

vk::DescriptorPool EditorRenderContext::CreateDescriptorPool(const DeviceContext& context, const std::uint32_t max_set_count) {
	std::vector<vk::DescriptorPoolSize> sizes = {
		vk::DescriptorPoolSize(vk::DescriptorType::eCombinedImageSampler, 16),
	};
	vk::DescriptorPoolCreateInfo create_info(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet, max_set_count, sizes);
	return context.GetDevice().createDescriptorPool(create_info);
}

EditorRenderContext::EditorRenderContext(const DeviceContext& context, vk::SurfaceKHR surface, vk::Extent2D swapchain_extent) {
	current_frame = 0;
	this->surface = std::make_unique<Surface>(context, surface);
	cmd_pool = context.GetDevice().createCommandPool(vk::CommandPoolCreateInfo(vk::CommandPoolCreateFlagBits::eResetCommandBuffer, context.GetGrpahicsQueueIndex()));
	swapchain = std::make_unique<Swapchain>(context, *this->surface, swapchain_extent);
	descriptor_pool = CreateDescriptorPool(context, swapchain->GetImageCount());
	render_pass = CreateRenderPass(context, swapchain->GetFormat());
	frames = EditorFrameContext::CreateFrameContext(context, cmd_pool, *swapchain, render_pass);
}

EditorRenderContext::~EditorRenderContext() {}

std::optional<std::tuple<EditorFrameContext&, uint32_t>> EditorRenderContext::WaitForNextFrame(const DeviceContext& context) {
	EditorFrameContext& frame = frames[current_frame];
	VK_CHECK(context.GetDevice().waitForFences({ frame.in_flight_fence }, vk::True, UINT64_MAX));
	context.GetDevice().resetFences({ frame.in_flight_fence });
	auto next_result = context.GetDevice().acquireNextImageKHR(*swapchain, UINT64_MAX, frame.image_available_semaphore);
	if (next_result.result == vk::Result::eErrorOutOfDateKHR) {
		return std::nullopt;
	}
	//std::printf("Current:%u, Next:%u\n", current_frame, next_result.value);
	return std::tie(frame, next_result.value);
}

void EditorRenderContext::BeginFrame(const DeviceContext& context, const EditorFrameContext& frame) {
	vk::CommandBuffer cmd = frame.command_buffer;
	cmd.reset();
	cmd.begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
}

void EditorRenderContext::SubmitFrame(const DeviceContext& context, const EditorFrameContext& frame) {
	frame.command_buffer.end();

	vk::PipelineStageFlags stage_mask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
	vk::SubmitInfo submit_info(
		frame.image_available_semaphore,
		stage_mask,
		frame.command_buffer,
		frame.render_finished_semaphore
	);
	context.GetGraphicsQueue().submit(submit_info, frame.in_flight_fence);
}

bool EditorRenderContext::PresentFrame(const DeviceContext& context, const EditorFrameContext& frame, const uint32_t image_index) {
	std::vector<vk::SwapchainKHR> swapchains = { *swapchain };
	vk::PresentInfoKHR present_info(frame.render_finished_semaphore, swapchains, image_index);
	vk::Result present_result = context.GetPresentQueue().presentKHR(&present_info);
	if (present_result == vk::Result::eErrorOutOfDateKHR || present_result == vk::Result::eSuboptimalKHR) {
		return true;
	}
	VK_CHECK(present_result);
	return false;
}

void EditorRenderContext::EndFrame() {
	current_frame = (current_frame + 1) % swapchain->GetImageCount();
}

void EditorRenderContext::CmdDrawUI(const EditorFrameContext& frame, EditorUI& ui) {
	vk::ClearValue clear_value = vk::ClearValue(vk::ClearColorValue());
	frame.command_buffer.beginRenderPass(vk::RenderPassBeginInfo(
		render_pass, frame.framebuffer,
		vk::Rect2D(vk::Offset2D(0, 0), swapchain->GetExtent()), clear_value),
		vk::SubpassContents::eInline);
	ui.CmdDraw(frame.command_buffer);
	frame.command_buffer.endRenderPass();
}

void EditorRenderContext::ResizeSwapchain(const DeviceContext& context, vk::Extent2D extent) {
	for (auto& frame : frames) {
		frame.Destroy(context);
	}
	if (swapchain) {
		swapchain->Destroy(context);
	}

	surface->ReacquireProperties(context);
	swapchain = std::make_unique<Swapchain>(context, *surface, extent);
	frames = EditorFrameContext::CreateFrameContext(context, cmd_pool, *swapchain, render_pass);
	current_frame = 0;
}

void EditorRenderContext::Destroy(const DeviceContext& context) {
	vk::Device device = context.GetDevice();
	for (auto& frame: frames) {
		frame.Destroy(context);
	}
	device.destroyRenderPass(render_pass);
	device.destroyDescriptorPool(descriptor_pool);
	device.destroyCommandPool(cmd_pool);
	swapchain->Destroy(context);
	surface->Destroy(context);
}
