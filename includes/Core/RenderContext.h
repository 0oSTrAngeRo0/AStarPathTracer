#pragma once

#include "Core/VulkanUsages.h"
#include "Core/Buffer.h"
#include "Core/Shader.h"
#include "Core/Instance.h"

#include <vulkan/vulkan.hpp>
#include <entt/entt.hpp>

class DeviceContext;
class Mesh;

class RenderContext {
public:
	std::vector<std::shared_ptr<ShaderBase>> shaders;
	std::vector<std::shared_ptr<Mesh>> meshes;

	Buffer vertex_buffer;
	Buffer index_buffer;
	Buffer material_buffer;

	vk::AccelerationStructureKHR tlas;
	Buffer tlas_buffer;
	Buffer tlas_instance_buffer;

	InstancesManager instances;

	Buffer constants_buffer;

	RenderContext(const DeviceContext& context);
	void Update(const DeviceContext& context, entt::registry& registry);
	void Destory(const DeviceContext& context);
private:
	void RecreateInstances(const DeviceContext& context, entt::registry& registry);
	void RecreateShaderBuffers(const DeviceContext& context);
	void UpdatePushConstants(const DeviceContext& context, entt::registry& registry);
};