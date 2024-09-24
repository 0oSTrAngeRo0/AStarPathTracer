#pragma once

#include <vulkan/vulkan.hpp>
#include "Core/RayTracingShaders.h"
#include "Engine/Guid.h"

class DeviceContext;
class RenderContext;
class Image;

class RendererPipeline {
public:
	RendererPipeline(const DeviceContext& context, const RenderContext& render, const vk::DescriptorPool descriptor_pool);
	void Destroy(const DeviceContext& context);
	void CmdDraw(const vk::CommandBuffer cmd, const RenderContext& render);
	void CmdCopyOutputTo(const vk::CommandBuffer cmd, const RenderContext& render, const vk::Image target);
	void UpdateDescriptorSet(const DeviceContext& context, const RenderContext& render) const {
		UpdateDescriptorSet(context, render, descriptor_set);
	}
	inline const std::unordered_map<Uuid, uint32_t>& GetShaderIndices() const { return shader_indices; }
private:
	/*enum class DescriptorBinding : uint8_t {
		eTlas,
		eOutputImage,
		eVertexPositionsBuffer,
		eVertexOthersBuffer,
		eIndicesBuffer,
		eMaterialsBuffer,
		eInstancesBuffer,
		eConstantsBuffer,
		eAccumulateImage,
		eTextures
	};*/
	static constexpr uint32_t MaxTextureCount = 1024;

	vk::PipelineLayout pipeline_layout;
	vk::Pipeline pipeline;
	std::unique_ptr<RayTracingShaders::BindingTable> shader_binding_table;
	vk::DescriptorSetLayout descriptor_set_layout;
	vk::DescriptorSet descriptor_set;
	std::unordered_map<Uuid, uint32_t> shader_indices;

	void CreateRayTracingPipelineLayout(const DeviceContext& context);
	void CreatePipelineAndBindingTable(const DeviceContext& context);

	static std::vector<vk::DescriptorSet> AllocateDescriptorSet(
		vk::Device device,
		vk::DescriptorPool pool,
		vk::DescriptorSetLayout layout
	);
	static void UpdateDescriptorSet(const DeviceContext& context, const RenderContext& render, const vk::DescriptorSet& set);
	static vk::DescriptorSetLayout CreateDescriptorSetLayout(const vk::Device device);
};