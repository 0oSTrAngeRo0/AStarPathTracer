#include "Engine/HostShaderManager.h"
#include "Engine/ShaderHostBuffer.h"
#include "Engine/Resources/ResourcesManager.h"
#include "Engine/Resources/ResourceData.h"

std::tuple<const Uuid, const size_t> HostShaderManager::RegisterMaterial(const Uuid& resource_id) {
	const ResourceBase& resource = ResourcesManager::GetInstance().GetResource(resource_id);
	const auto result = GetMaterialBaseFunctionRegistry::Get(resource.GetResourceType());
	if (!result.has_value()) {
		throw std::runtime_error("Invalie resource type");
	}
	const auto& material = result.value()(resource);
	HostShader& shader = GetShader(material.shader_id);
	size_t index = material.RegisterToShader(resource_id, shader);
	return std::tie(material.shader_id, index);
}

std::tuple<const Uuid, const size_t> HostShaderManager::GetMaterialRuntimeData(const Uuid& resource_id) const {
	const ResourceBase& resource = ResourcesManager::GetInstance().GetResource(resource_id);
	const auto result = GetMaterialBaseFunctionRegistry::Get(resource.GetResourceType());
	if (!result.has_value()) {
		throw std::runtime_error("Invalie resource type");
	}
	const auto& material = result.value()(resource);
	const auto& index = GetShader(material.shader_id).GetIndex(resource_id);
	return std::tie(material.shader_id, index);
}

std::vector<std::reference_wrapper<const HostShader>> HostShaderManager::GetAllShaders() const {
	std::vector<std::reference_wrapper<const HostShader>> data;
	for (const auto& pair : host_shaders) {
		data.emplace_back(std::ref(*pair.second));
	}
	return data;
}

void HostShaderManager::RegisterShader(const Uuid& id, const size_t stride) {
	host_shaders.insert(std::make_pair(id, std::move(std::make_unique<HostShader>(id, stride))));
}

inline const HostShader& HostShaderManager::GetShader(const Uuid& id) const {
	return *host_shaders.at(id);
}

inline HostShader& HostShaderManager::GetShader(const Uuid& id) {
	return *host_shaders.at(id);
}
