#pragma once

#include <crossguid/guid.hpp>
typedef xg::Guid Uuid;

#include <vector>
#include "Engine/ShaderHostBuffer.h"
#include <glm/glm.hpp>

#define ASSET_SOURCES_DIR "D:/C++/Projects/PathTracer/EngineRuntime/AssetSources"
#define RESOURCES_DIR "D:/C++/Projects/PathTracer/EngineRuntime/Resources"

struct SimpleLitMaterialData {
	glm::vec4 color;
};

/// <summary>
/// 不允许实例化，只能实例化其子类
/// </summary>
class ResourceBase {
public:
	std::string resource_type;
	Uuid uuid;
	std::vector<Uuid> references;

	ResourceBase(const std::string& type) :resource_type(type), uuid(xg::newGuid()) {}

	static std::string Serialize(const ResourceBase& data);
	static std::unique_ptr<ResourceBase> Deserialize(const std::string& str);
	virtual ~ResourceBase() = default;
};

template <typename TData>
class Resource : public ResourceBase {
public:
	Resource();
	TData resource_data;
};
