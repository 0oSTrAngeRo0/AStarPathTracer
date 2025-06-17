#pragma once

#include "Application/Renderer/RenderContext.h"
#include "Application/Renderer/RendererPipeline.h"

class Renderer {
private:
	RenderContext context;
	RendererPipeline pipeline;

public:
	Renderer(const DeviceContext& device) : context(device), pipeline(device, context) {}
	void ResizeOutput(const DeviceContext &device, vk::Extent2D extent, vk::Format format) {
		context.RecreateOutputImage(device, extent, format);
	}
	void PrepareRenderData(const DeviceContext& device, entt::registry& registry) {
		context.Update(device, registry, pipeline.GetShaderIndices());
		pipeline.UpdateDescriptorSet(device, context);
	}
	void CmdDraw(vk::CommandBuffer cmd) {
		pipeline.CmdDraw(cmd, context);
	}
	void CmdCopyOutputTo(vk::CommandBuffer cmd, vk::Image image) {
		pipeline.CmdCopyOutputTo(cmd, context, image);
	}
	void Destory(const DeviceContext& device) {
		pipeline.Destroy(device);
		context.Destory(device);
	}
};