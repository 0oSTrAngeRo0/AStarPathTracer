#pragma once

#include <crossguid/guid.hpp>
typedef xg::Guid Uuid;

#include <vector>

#define ASSET_SOURCES_DIR "D:\C++\Projects\PathTracer\EngineRuntime\AssetSources"
#define RESOURCES_DIR "D:\C++\Projects\PathTracer\EngineRuntime\Resources"

enum ResourceType {
	eObj,
	eMaterial
};

/// <summary>
/// 不允许实例化，只能实例化其子类
/// </summary>
class ResourceBase {
public:
	ResourceType resource_type;
	Uuid uuid;
	std::vector<Uuid> references;

	static std::string Serialize(const ResourceBase& data);
	static std::unique_ptr<ResourceBase> Deserialize(const std::string& str);
	virtual ~ResourceBase() = default;
};

template <typename TData>
class ResourceTemplate : public ResourceBase {
public:
	TData resource_data;
};

class ObjResourceData {
public:
	std::string path; // relative path
	// other import settings
};

class MaterialResourceData {
public:
	std::string material_type;
};
