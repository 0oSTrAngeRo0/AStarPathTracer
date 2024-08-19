#pragma once

#include "Core/VulkanUsages.h"
#include "Core/Buffer.h"
#include "Core/Shader.h"
#include "Core/Instance.h"

#include <vulkan/vulkan.hpp>
#include <entt/entt.hpp>

#include "Engine/Resources/Resources.h"
#include "Engine/HostBuffer.h"
#include "Application/Renderer/MeshPool.h"
#include "Application/Renderer/MaterialPool.h"

class DeviceContext;
class Mesh;

class RenderContext {
public:
	RenderContext(const DeviceContext& context);
	void Update(const DeviceContext& context, entt::registry& registry);
	void Destory(const DeviceContext& context);
	inline const Buffer& GetVertexPositionBuffer() const { return mesh_pool.GetVertexPositionBuffer(); }
	inline const Buffer& GetVertexOtherBuffer() const { return mesh_pool.GetVertexOtherBuffer(); }
	inline const Buffer& GetIndexBuffer() const { return mesh_pool.GetIndexBuffer(); }
	inline const Buffer& GetMaterialBuffer() const { return material_pool.GetMainBuffer(); }
	inline const Buffer& GetInstancesBuffer() const { return instances_buffer; }
	inline const Buffer& GetConstantsBuffer() const { return constants_buffer; }
	inline const vk::AccelerationStructureKHR GetTlas() const { return tlas; }
private:
	MaterialPool material_pool;
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

	void UploadMeshes(const DeviceContext& context, entt::registry& registry);
	void UploadMaterials(const DeviceContext& context, entt::registry& registry);
	void RecreateInstances(const DeviceContext& context, entt::registry& registry);
	void UpdatePushConstants(const DeviceContext& context, entt::registry& registry);
};