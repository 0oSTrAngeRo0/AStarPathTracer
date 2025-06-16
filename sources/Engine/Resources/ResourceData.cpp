#include "Engine/Resources/ResourceData.h"
#include "Engine/Resources/Resources.h"
#include "Engine/Resources/ResourceRegistry.h"
#include "Engine/Json/Resource.h"
#include "Engine/ShaderHostBuffer.h"
#include "Engine/HostShaderManager.h"
#include "Utilities/MacroUtilities.h"



#define SHADER_RESOURCE_AFTER_DESERIALIZE(resource) \
do { \
	if (resource.resource_data.shader_stage != vk::ShaderStageFlagBits::eClosestHitKHR) break; \
	HostShaderManager::GetInstance().RegisterShader(resource.uuid, resource.resource_data.instance_stride); \
} while(false)
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
REGISTER_RESOURCE_DESERIALIZER_WITHAFTER(ShaderResourceData, SHADER_RESOURCE_AFTER_DESERIALIZE);


template <> const std::string& Resource<TextureResourceData>::GetResourceTypeStatic() {
	static std::string type = "Texture";
	return type;
}
JSON_SERIALIZER(TextureResourceData, <>, path);
REGISTER_RESOURCE_SERIALIZER(TextureResourceData)
REGISTER_RESOURCE_DESERIALIZER(TextureResourceData)
