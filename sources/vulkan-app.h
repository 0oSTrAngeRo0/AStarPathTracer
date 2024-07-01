#pragma once

#include "Core/DeviceContext.h"
#include <vulkan/vulkan.hpp>
#include "event-registry.h"
#include "main-window.h"
#include <vector>
#include "Core/Mesh.h"
#include "ray-tracing-shader-binding-table.h"
#include "Core/Image.h"

class Scene;

class VulkanApp {
public:
	struct SwapchainRuntimeInfo {
		vk::SwapchainKHR swapchain;
		std::vector<vk::Image> images;
		vk::Format format;
		vk::Extent2D extent;
		std::vector<vk::ImageView> image_views;
		uint32_t count;
	};

private:
	AppConfig config;
	std::unique_ptr<DeviceContext> context;
	SwapchainRuntimeInfo swapchain_info;
	vk::PipelineLayout pipeline_layout;
	vk::Pipeline ray_tracing_pipeline;
	vk::CommandPool command_pool;
	std::vector<vk::CommandBuffer> command_buffers;
	vk::Semaphore image_available_semaphore;
	vk::Semaphore render_finished_semaphore;
	vk::Fence in_flight_fence;
	std::unique_ptr<RayTracingBindingTable> shader_binding_table;
	vk::DescriptorPool descriptor_pool;
	vk::DescriptorSetLayout descriptor_set_layout;
	vk::DescriptorSet descriptor_set;
	std::unique_ptr<Image> rt_image;
	vk::ImageView rt_image_view;
	std::unique_ptr<Scene> scene;

public:
	VulkanApp(const AppConfig& config, const Window& main_window);
	~VulkanApp();
	void Update();

private:
	void CreateSwapchain();
	void CreateRayTracingPipelineLayout();
	void CreateSyncObjects();
	void CreateRayTracingPipeline();
	void UploadDescriptorSet();
	void CreateCommandBuffers();
};


