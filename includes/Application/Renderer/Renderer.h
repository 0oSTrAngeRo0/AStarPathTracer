#pragma once

#include <vulkan/vulkan.hpp>

class DeviceContext;
class Swapchain;

class Renderer {
public:
	struct FrameData {
		vk::CommandBuffer command_buffer;
		uint32_t image_index;
		vk::Image image;

		FrameData(const vk::CommandBuffer command_buffer, uint32_t image_index, vk::Image image) :
			command_buffer(command_buffer), image_index(image_index), image(image) {}
	};

	Renderer(const DeviceContext& context);
	void Destroy(const DeviceContext& context);
	~Renderer();
	void WaitForNextFrame(const DeviceContext& context);
	const FrameData BeginFrame(const DeviceContext& context);
	void EndFrame(const DeviceContext& context, const FrameData& frame_data);
	void ResizeSwapchain(const DeviceContext& context, const vk::Extent2D extent);

	inline const vk::DescriptorPool GetDescriptorPool() const { return descriptor_pool; }
private:
	std::unique_ptr<Swapchain> swapchain;
	vk::CommandPool command_pool;
	vk::Semaphore image_available_semaphore;
	vk::Semaphore render_finished_semaphore;
	vk::Fence in_flight_fence;
	vk::DescriptorPool descriptor_pool;

	void CreateSyncObjects(const DeviceContext& context);
	void CreateDescriptorPool(const DeviceContext& context);
};
