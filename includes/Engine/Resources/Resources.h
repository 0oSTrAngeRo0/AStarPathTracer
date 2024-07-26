#pragma once

#include <crossguid/guid.hpp>
typedef xg::Guid Uuid;

#include <vector>
#include "Engine/ShaderHostBuffer.h"
#include <glm/glm.hpp>

#define ASSET_SOURCES_DIR "D:/C++/Projects/PathTracer/EngineRuntime/AssetSources"
#define RESOURCES_DIR "D:/C++/Projects/PathTracer/EngineRuntime/Resources"

enum class ResourceType {
	eUnknown,
	eObj,
	eMaterial
};

enum class MaterialType {
	eUnknown,
	eSimpleLit
};

struct SimpleLitMaterialData {
	glm::vec4 color;
};

/// <summary>
/// 不允许实例化，只能实例化其子类
/// </summary>
class ResourceBase {
public:
	ResourceType resource_type;
	Uuid uuid;
	std::vector<Uuid> references;

	ResourceBase(ResourceType type) :resource_type(type), uuid(xg::newGuid()) {}

	static std::string Serialize(const ResourceBase& data);
	static std::unique_ptr<ResourceBase> Deserialize(const std::string& str);
	virtual ~ResourceBase() = default;
};

template <typename TData>
class ResourceTemplate : public ResourceBase {
public:
	ResourceTemplate();
	TData resource_data;
};

class ObjResourceData {
public:
	std::string path; // relative path
	// other import settings
};

class MaterialResourceDataBase {
public:
	MaterialType material_type;
	MaterialResourceDataBase(MaterialType type) : material_type(type) {}
};

template <typename T>
class MaterialResourceData : public MaterialResourceDataBase {
private:
	std::shared_ptr<HostBufferVisitor<T>> material_visitor;
public:
	T material_data;

	MaterialResourceData();
};
