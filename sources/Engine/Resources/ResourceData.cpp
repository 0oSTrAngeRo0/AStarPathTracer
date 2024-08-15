#include "Engine/Resources/ResourceData.h"
#include "Engine/Resources/Resources.h"
#include "Engine/Resources/ResourceRegistry.h"
#include "Engine/Resources/JsonSerializer.h"
#include "Engine/ShaderHostBuffer.h"
#include "Engine/HostShaderManager.h"
#include "Utilities/MacroUtilities.h"

template <> const std::string& Resource<ObjResourceData>::GetResourceTypeStatic() {
	static std::string type = "Obj";
	return type;
}
JSON_SERIALIZER(ObjResourceData, <>, path);
REGISTER_RESOURCE_SERIALIZER(ObjResourceData);
REGISTER_RESOURCE_DESERIALIZER(ObjResourceData, ASTAR_DO_NOTHING);

template <> const std::string& Resource<ShaderResourceData>::GetResourceTypeStatic() {
	static std::string type = "Shader";
	return type;
}
JSON_ENUM_SERIALIZER(vk::ShaderStageFlagBits, {
	{ vk::ShaderStageFlagBits::eClosestHitKHR, "ClosestHitKHR" },
	{ vk::ShaderStageFlagBits::eRaygenKHR, "RaygenKHR" },
	{ vk::ShaderStageFlagBits::eMissKHR, "MissKHR" },
	{ vk::ShaderStageFlagBits::eCallableKHR, "CallableKHR" },
	{ vk::ShaderStageFlagBits::eAnyHitKHR, "AnyHitKHR" }
});
JSON_SERIALIZER(ShaderResourceData, <>, source_code_path, compiled_code_path, shader_stage, instance_stride, entry_function);
REGISTER_RESOURCE_SERIALIZER(ShaderResourceData);
REGISTER_RESOURCE_DESERIALIZER(ShaderResourceData, \
[](const Resource<ShaderResourceData>& resource) { \
	if (resource.resource_data.shader_stage != vk::ShaderStageFlagBits::eClosestHitKHR) return; \
	HostShaderManager::GetInstance().RegisterShader(resource.uuid, resource.resource_data.instance_stride); \
});

JSON_SERIALIZER(MaterialResourceData<TMatData>, <typename TMatData>, material_data, shader_id);

template <> const std::string& Resource<MaterialResourceData<SimpleLitMaterialData>>::GetResourceTypeStatic() {
	static std::string type = "MaterialSimpleLit";
	return type;
}
JSON_SERIALIZER(SimpleLitMaterialData, <>, color);
REGISTER_RESOURCE_SERIALIZER(MaterialResourceData<SimpleLitMaterialData>);
REGISTER_RESOURCE_DESERIALIZER(MaterialResourceData<SimpleLitMaterialData>, ASTAR_DO_NOTHING);
REGISTER_GET_BASE_MATERIAL_DATA(MaterialResourceData<SimpleLitMaterialData>);
