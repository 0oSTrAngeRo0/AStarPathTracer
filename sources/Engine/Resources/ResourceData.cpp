#include "Engine/Resources/ResourceData.h"
#include "Engine/Resources/Resources.h"
#include "Engine/Resources/ResourceRegistry.h"
#include "Engine/Json/Resource.h"
#include "Engine/ShaderHostBuffer.h"
#include "Engine/HostShaderManager.h"
#include "Utilities/MacroUtilities.h"

JSON_ENUM_SERIALIZER(vk::ShaderStageFlagBits, {
	{ vk::ShaderStageFlagBits::eClosestHitKHR, "ClosestHitKHR" },
	{ vk::ShaderStageFlagBits::eRaygenKHR, "RaygenKHR" },
	{ vk::ShaderStageFlagBits::eMissKHR, "MissKHR" },
	{ vk::ShaderStageFlagBits::eCallableKHR, "CallableKHR" },
	{ vk::ShaderStageFlagBits::eAnyHitKHR, "AnyHitKHR" }
});
JSON_SERIALIZER(ShaderResourceData, <>, source_code_path, compiled_code_path, shader_stage, instance_stride, entry_function);


static void ShaderResourceAfterDeserialize(const Resource<ShaderResourceData>& resource) {
	if (resource.resource_data.shader_stage != vk::ShaderStageFlagBits::eClosestHitKHR) return;
	HostShaderManager::GetInstance().RegisterShader(resource.uuid, resource.resource_data.instance_stride);
}

static void Register() {
	ResourceSerializeRegistry::Register<ShaderResourceData>();
	ResourceDeserializerRegistry::RegisterWithAfter<ShaderResourceData>(ShaderResourceAfterDeserialize);


}

ASTAR_BEFORE_MAIN(Register());