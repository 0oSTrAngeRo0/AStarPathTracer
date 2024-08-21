#pragma once

#include <vulkan/vulkan.hpp>
#include "Core/RayTracingShaders.h"

class DeviceContext;
class RenderContext;
class Image;

class RendererPipeline {
private:
	vk::PipelineLayout pipeline_layout;
	vk::Pipeline pipeline;
	std::unique_ptr<RayTracingShaders::BindingTable> shader_binding_table;
	vk::DescriptorSetLayout descriptor_set_layout;
	vk::DescriptorSet descriptor_set;

public:
	RendererPipeline(const DeviceContext& context, const RenderContext& render, const vk::DescriptorPool descriptor_pool);
	void Destroy(const DeviceContext& context);
	void CmdDraw(const vk::CommandBuffer cmd, const RenderContext& render);
	void CmdCopyOutputTo(const vk::CommandBuffer cmd, const RenderContext& render, const vk::Image target);
	void UpdateDescriptorSet(const DeviceContext& context, const RenderContext& render) const {
		UpdateDescriptorSet(context, render, descriptor_set);
	}
private:
	void CreateRayTracingPipelineLayout(const DeviceContext& context);
	void CreatePipelineAndBindingTable(const DeviceContext& context);

	static void UpdateDescriptorSet(const DeviceContext& context, const RenderContext& render, const vk::DescriptorSet& set);

	static vk::DescriptorSetLayout CreateDescriptorSetLayout(const vk::Device device);
};