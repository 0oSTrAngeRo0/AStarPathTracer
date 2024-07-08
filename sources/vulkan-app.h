#pragma once

#include "Core/DeviceContext.h"
#include <vulkan/vulkan.hpp>
#include "event-registry.h"
#include "main-window.h"
#include <vector>
#include "Core/Mesh.h"
#include "ray-tracing-shader-binding-table.h"
#include "Core/Image.h"
#include "entt/entt.hpp"
#include "Core/Swapchain.h"

class RenderContext;

class VulkanApp {
private:
	std::unique_ptr<Swapchain> swapchain;
	vk::PipelineLayout pipeline_layout;
	vk::Pipeline ray_tracing_pipeline;
	vk::CommandPool command_pool;
	vk::Semaphore image_available_semaphore;
	vk::Semaphore render_finished_semaphore;
	vk::Fence in_flight_fence;
	std::unique_ptr<RayTracingBindingTable> shader_binding_table;
	vk::DescriptorPool descriptor_pool;
	vk::DescriptorSetLayout descriptor_set_layout;
	vk::DescriptorSet descriptor_set;
	std::unique_ptr<Image> rt_image;
	vk::ImageView rt_image_view;

public:
	VulkanApp(const DeviceContext& context, const RenderContext& render);
	void Destroy(const DeviceContext& context);
	void Draw(const DeviceContext& context, const RenderContext& render);

private:
	void CreateRayTracingPipelineLayout(const DeviceContext& context);
	void CreateSyncObjects(const DeviceContext& context);
	void CreateRayTracingPipeline(const DeviceContext& context);
	void UploadDescriptorSet(const DeviceContext& context, const RenderContext& render);
	vk::CommandBuffer CreateFrameCommandBuffer(const DeviceContext& context, const vk::CommandPool pool, const uint32_t index);
};


