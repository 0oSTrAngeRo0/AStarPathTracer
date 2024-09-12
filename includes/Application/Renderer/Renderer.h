#pragma once

#include <vulkan/vulkan.hpp>

class DeviceContext;
class Swapchain;
class Surface;

class Renderer {
public:
	struct FrameData {
		vk::CommandBuffer command_buffer;
		uint32_t image_index;
		vk::Image image;

		FrameData(const vk::CommandBuffer command_buffer, uint32_t image_index, vk::Image image) :
			command_buffer(command_buffer), image_index(image_index), image(image) {}
	};

	Renderer(const DeviceContext& context, vk::SurfaceKHR surface);
	void Destroy(const DeviceContext& context);
	~Renderer();
	void WaitForNextFrame(const DeviceContext& context);
	const FrameData BeginFrame(const DeviceContext& context);
	void EndFrame(const DeviceContext& context, const FrameData& frame_data);
	void ResizeSwapchain(const DeviceContext& context, const vk::Extent2D extent);
	void RefreshSurfaceData(const DeviceContext& context);

	inline const vk::DescriptorPool GetDescriptorPool() const { return descriptor_pool; }
	const vk::Format GetSwapchainFormat() const;
private:
	std::unique_ptr<Swapchain> swapchain;
	std::unique_ptr<Surface> surface;
	vk::CommandPool command_pool;
	vk::Semaphore image_available_semaphore;
	vk::Semaphore render_finished_semaphore;
	vk::Fence in_flight_fence;
	vk::DescriptorPool descriptor_pool;

	void CreateSyncObjects(const DeviceContext& context);
	void CreateDescriptorPool(const DeviceContext& context);
};
