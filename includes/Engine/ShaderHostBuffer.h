#pragma once

#include "Engine/HostBuffer.h"
#include "Utilities/Singleton.h"
#include "Engine/Resources/ResourcesManager.h"
#include "Engine/StaticRegistry.h"

class HostShaderBase {
public:
	virtual const size_t GetMaterialIndex(const Uuid& id) const = 0;
	virtual const Uuid& GetId() const = 0;
	virtual const std::vector<std::byte>& GetData() const = 0;
	virtual const bool IsDirty() const = 0;
};

class HostShaderManager : 
	public Singleton<HostShaderManager>, 
	public StaticRegistry<std::string, std::function<std::tuple<const Uuid, const Uuid, size_t>(const ResourceBase&)>> {
public:

	std::tuple<const Uuid, const size_t> GetMaterialRuntimeData(const Uuid& resource_id) const;
	std::vector<std::reference_wrapper<const HostShaderBase>> GetAllShaders() const;
	void RegisterShader(const Uuid& id, const HostShaderBase& shader);
private:
	std::unordered_map<Uuid, std::reference_wrapper<const HostShaderBase>> host_shaders;
};

#define REGISTER_MATERIAL_RUNTIME_DATA(type) \
static bool ASTAR_UNIQUE_VARIABLE_NAME(material_runtime_data_register_) = (HostShaderManager::Register(Resource<type>::GetResourceTypeStatic(), \
	[](const ResourceBase& resource) { \
		const auto& data = static_cast<const Resource<type>&>(resource).resource_data; \
		return std::make_tuple(data.GetShaderId(), data.GetMaterialId(), data.GetMaterialIndex()); \
	}), true); \


template <typename T>
class HostShaderBuffer : public HostShaderBase, public HostBuffer<T>, public Singleton<HostShaderBuffer<T>> {
public:
	inline const size_t GetMaterialIndex(const Uuid& id) const override { return HostBuffer<T>::GetIndex(id); }
	inline const std::vector<std::byte>& GetData() const override { return  reinterpret_cast<const std::vector<std::byte>&>(HostBuffer<T>::GetHostBuffer()); }
	inline const bool IsDirty() const override { return HostBuffer<T>::IsDirty(); }
	inline const Uuid& GetId() const { return GetIdStatic(); }
	static const Uuid& GetIdStatic() {
		static Uuid id = xg::newGuid();
		return id;
	}
	HostShaderBuffer() {
		HostShaderManager::GetInstance().RegisterShader(GetIdStatic(), *this);
	}
};
