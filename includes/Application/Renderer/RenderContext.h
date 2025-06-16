#pragma once

#include <memory>

#include "Core/VulkanUsages.h"
#include "Core/Buffer.h"
#include "Core/Shader.h"
#include "Core/Instance.h"

#include <vulkan/vulkan.hpp>
#include <entt/entt.hpp>

#include "Engine/Resources/Resources.h"
#include "Engine/HostShaderManager.h"
#include "Application/Renderer/MeshPool.h"
#include "Application/Renderer/MaterialPool.h"
#include "Application/Renderer/TexturePool.h"
#include "Core/Image.h"

#include "Engine/StaticRegistry.h"
#include "Utilities/MacroUtilities.h"

class DeviceContext;
class Mesh;

struct MaterialVisiableContext {
public:
	TexturePool& textures;
	HostShaderManager& shader_manager;
	MaterialVisiableContext(TexturePool& textures, HostShaderManager& shader_manager) : 
		textures(textures), shader_manager(shader_manager) {}
};

class UpdateMaterialDataRegistry : public StaticFunctionRegistry<ResourceTypeId, void(const ResourceBase&, const MaterialVisiableContext&)> {
public:
	using Base = StaticFunctionRegistry<ResourceTypeId, void(const ResourceBase&, const MaterialVisiableContext&)>;
	template <typename TResource, typename TRuntimeData>
	static TRuntimeData UpdateMaterialData(const Resource<TResource>& resource, const MaterialVisiableContext& context);
	
	template <typename TResource, typename TRuntimeData> static void Register() {
		Base::Register(Resource<TResource>::type_id, 
		[](const ResourceBase& resource, const MaterialVisiableContext& context) { 
			auto& resource_impl = static_cast<const Resource<TResource>&>(resource); 
			auto& shader = context.shader_manager.GetShader(resource_impl.resource_data.shader_id); 
			TRuntimeData runtime_data = UpdateMaterialDataRegistry::UpdateMaterialData<TResource, TRuntimeData>(resource_impl, context); 
			if (shader.IsIdValid(resource_impl.uuid)) 
				shader.SetValue(resource_impl.uuid, runtime_data); 
			else 
				shader.EmplaceValue(resource_impl.uuid, runtime_data); 
		});
	}
};

class RenderContext {
public:
	RenderContext(const DeviceContext& context);
	void Update(
		const DeviceContext& context, 
		entt::registry& registry, 
		const std::unordered_map<Uuid, uint32_t>& shader_indices // sbt data
	);
	void Destory(const DeviceContext& context);
	void RecreateOutputImage(const DeviceContext& context, const vk::Extent2D extent, vk::Format format);

	inline const Buffer& GetVertexPositionBuffer() const { return mesh_pool.GetVertexPositionBuffer(); }
	inline const Buffer& GetVertexOtherBuffer() const { return mesh_pool.GetVertexOtherBuffer(); }
	inline const Buffer& GetIndexBuffer() const { return mesh_pool.GetIndexBuffer(); }
	inline const Buffer& GetMaterialBuffer() const { return material_pool.GetMainBuffer(); }
	inline const Buffer& GetInstancesBuffer() const { return instances_buffer; }
	inline const Buffer& GetConstantsBuffer() const { return constants_buffer; }
	inline const vk::ImageView GetOutputImageView() const { return output_image->output_image_view; }
	inline const Image& GetOutputImage() const { return output_image->output_image; }
	inline const vk::ImageView GetAccumulateImageView() const { return output_image->accumulate_image_view; }
	inline const Image& GetAccumulateImage() const { return output_image->accumulate_image; }
	inline const vk::Extent2D GetOutputImageExtent() const { return output_image->extent; }
	inline const vk::AccelerationStructureKHR GetTlas() const { return tlas; }
	inline const bool IsTexturesDirty() const { return texture_pool.IsDirty(); }
	inline const std::vector<vk::DescriptorImageInfo> GetTextures() const { return texture_pool.GetDescriptorData(); }
private:
	MaterialPool material_pool;
	TexturePool texture_pool;
	MeshPool mesh_pool;

	vk::AccelerationStructureKHR tlas;
	Buffer tlas_buffer;
	Buffer tlas_instance_buffer;

	Buffer instances_buffer;

	Buffer constants_buffer;

	struct ConstantsData {
		glm::mat4 view_inverse;
		glm::mat4 projection_inverse;
		uint32_t sample_per_pixel;
	};
	ConstantsData constants_data;

	struct OutputImage {
		Image output_image;
		vk::ImageView output_image_view;
		Image accumulate_image;
		vk::ImageView accumulate_image_view;
		vk::Extent2D extent;

		OutputImage(const DeviceContext& context, const vk::Extent2D extent, vk::Format format);
		void Destroy(const DeviceContext& context);
	};
	std::unique_ptr<OutputImage> output_image;

	void UploadMeshes(const DeviceContext& context, entt::registry& registry);
	void UploadMaterials(
		const DeviceContext& context, 
		entt::registry& registry,
		const std::unordered_map<Uuid, uint32_t>& shader_indices
	);
	void RecreateInstances(
		const DeviceContext& context, 
		entt::registry& registry, 
		const std::unordered_map<Uuid, uint32_t>& shader_indices
	);
	void UpdatePushConstants(const DeviceContext& context, entt::registry& registry);
	static vk::TransformMatrixKHR GetTransformMatrixKHR(const glm::mat4 transform);
};