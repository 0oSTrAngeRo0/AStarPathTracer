#pragma once

#include <vulkan/vulkan.hpp>
#include "Core/Swapchain.h"

class DeviceContext;

class Renderer {
public:
	struct FrameData {
		bool should_skip;
		vk::CommandBuffer command_buffer;
		uint32_t image_index;
		vk::Image image;

		FrameData() : should_skip(true) {}

		FrameData(const vk::CommandBuffer command_buffer, uint32_t image_index, vk::Image image) :
			command_buffer(command_buffer), image_index(image_index), image(image), should_skip(false) {}
	};

	Renderer(const DeviceContext& context);
	void Destroy(const DeviceContext& context);
	const FrameData BeginFrame(const DeviceContext& context);
	void EndFrame(const DeviceContext& context, const FrameData& frame_data);
	inline const vk::DescriptorPool GetDescriptorPool() const { return descriptor_pool; }

private:
	std::unique_ptr<Swapchain> swapchain;
	vk::CommandPool command_pool;
	vk::Semaphore image_available_semaphore;
	vk::Semaphore render_finished_semaphore;
	vk::Fence in_flight_fence;
	vk::DescriptorPool descriptor_pool;
	bool should_resize_swapchain;

	void CreateSyncObjects(const DeviceContext& context);
	void CreateDescriptorPool(const DeviceContext& context);
};
