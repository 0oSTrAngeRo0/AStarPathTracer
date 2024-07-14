#pragma once

#include "Core/DeviceContext.h"
#include <vulkan/vulkan.hpp>
#include "event-registry.h"
#include "Application/GlfwWindow.h"
#include <vector>
#include "Core/Mesh.h"
#include "Core/RayTracingShaderBindingTable.h"
#include "Core/Image.h"
#include "entt/entt.hpp"
#include "Core/Swapchain.h"

class RenderContext;

class Renderer {
private:
	std::unique_ptr<Swapchain> swapchain;
	vk::PipelineLayout pipeline_layout;
	vk::Pipeline ray_tracing_pipeline;
	vk::CommandPool command_pool;
	vk::Semaphore image_available_semaphore;
	vk::Semaphore render_finished_semaphore;
	vk::Fence in_flight_fence;
	std::unique_ptr<RayTracingShaderBindingTable> shader_binding_table;
	vk::DescriptorPool descriptor_pool;
	vk::DescriptorSetLayout descriptor_set_layout;
	vk::DescriptorSet descriptor_set;
	std::unique_ptr<Image> rt_image;
	vk::ImageView rt_image_view;

public:
	Renderer(const DeviceContext& context, const RenderContext& render);
	void Destroy(const DeviceContext& context);
	void Draw(const DeviceContext& context, const RenderContext& render);

private:
	void CreateRayTracingPipelineLayout(const DeviceContext& context);
	void CreateSyncObjects(const DeviceContext& context);
	void CreateRayTracingPipeline(const DeviceContext& context);
	void UploadDescriptorSet(const DeviceContext& context, const RenderContext& render);
	vk::CommandBuffer CreateFrameCommandBuffer(const DeviceContext& context, const vk::CommandPool pool, const uint32_t index);

	static void CmdInsertImageBarrier(const vk::CommandBuffer cmd, vk::ImageMemoryBarrier barrier);
	static void UpdateDescriptorSet(const DeviceContext& context, const RenderContext& render, const vk::DescriptorSet& set);
	static vk::DescriptorPool CreateDescriptorPool(const vk::Device device);
	static vk::DescriptorSetLayout CreateDescriptorSetLayout(const vk::Device device);
};


