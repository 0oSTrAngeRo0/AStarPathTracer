#include "Engine/ShaderHostBuffer.h"
#include "Engine/Resources/ResourcesManager.h"

std::tuple<const Uuid, const size_t> HostShaderManager::GetMaterialRuntimeData(const Uuid& resource_id) const {
	const ResourceBase& resource = ResourcesManager::GetInstance().GetResource(resource_id);
	const auto result = Get(resource.GetResourceType());
	if (!result.has_value()) {
		throw std::runtime_error("Invalie resource type");
	}
	const auto& [shader, material, index] = result.value()(resource);
	return std::tie(shader, index);
}

std::vector<std::reference_wrapper<const HostShaderBase>> HostShaderManager::GetAllShaders() const {
	std::vector<std::reference_wrapper<const HostShaderBase>> data;
	for (const auto& pair : host_shaders) {
		data.emplace_back(pair.second);
	}
	return data;
}

void HostShaderManager::RegisterShader(const Uuid& id, const HostShaderBase& shader) {
	if (host_shaders.contains(id)) {
		throw std::runtime_error("Shader is confict: [" + id.str() + "]");
	}
	host_shaders.insert(std::make_pair(id, std::ref(shader)));
}
