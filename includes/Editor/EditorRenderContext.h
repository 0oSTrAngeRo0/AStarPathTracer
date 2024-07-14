#pragma once

#include <vulkan/vulkan.hpp>

class DeviceContext;
class Swapchain;
class EditorUI;

class EditorFrameContext {
private:
	EditorFrameContext(
		const DeviceContext& context,
		const vk::CommandBuffer cmd,
		const vk::Framebuffer framebuffer
	);
public:
	vk::CommandBuffer command_buffer;
	vk::Fence in_flight_fence;
	vk::Semaphore image_available_semaphore;
	vk::Semaphore render_finished_semaphore;
	vk::Framebuffer framebuffer;
	void Destroy(const DeviceContext& context);

	static std::vector<EditorFrameContext> CreateFrameContext(
		const DeviceContext& context,
		const vk::CommandPool pool,
		const Swapchain& swapchain,
		const vk::RenderPass& pass
	);
};

class EditorRenderContext {
private:
	vk::RenderPass render_pass;
	std::unique_ptr<Swapchain> swapchain;
	vk::DescriptorPool descriptor_pool;
	vk::CommandPool cmd_pool;
	uint32_t current_frame;
	std::vector<EditorFrameContext> frames;

	static vk::RenderPass CreateRenderPass(const DeviceContext& context, const vk::Format format);
	static vk::DescriptorPool CreateDescriptorPool(const DeviceContext& context);
public:
	EditorRenderContext(const DeviceContext& context);
	inline const Swapchain& GetSwapchain() const { return *swapchain; }
	inline const vk::RenderPass GetRenderPass() const { return render_pass; }
	inline const vk::DescriptorPool GetDescriptorPool() const { return descriptor_pool; }

	std::tuple<EditorFrameContext&, const uint32_t> BeginFrame(const DeviceContext& context);
	void SubmitFrame(const DeviceContext& context, const EditorFrameContext& frame);
	void PresentFrame(const DeviceContext& context, const EditorFrameContext& frame, const uint32_t image_index);
	void EndFrame();
	void CmdDrawUI(const EditorFrameContext& frame, EditorUI& ui);

	void Destroy(const DeviceContext& context);
};