#pragma once

#include "Engine/Guid.h"
#include <unordered_map>
#include "Utilities/Singleton.h"

class HostShader;

class HostShaderManager : public Singleton<HostShaderManager> {
public:
	std::tuple<const Uuid, const size_t> RegisterMaterial(const Uuid& resource_id);
	std::tuple<const Uuid, const size_t> GetMaterialRuntimeData(const Uuid& resource_id) const;
	std::vector<std::reference_wrapper<const HostShader>> GetAllShaders() const;
	void RegisterShader(const Uuid& id, const size_t stride);
	const HostShader& GetShader(const Uuid& id) const;
	HostShader& GetShader(const Uuid& id);
private:
	std::unordered_map<Uuid, std::unique_ptr<HostShader>> host_shaders;
};