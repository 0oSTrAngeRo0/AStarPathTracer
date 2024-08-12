#pragma once

#include <Engine/Guid.h>
#include <vector>

#define ASSET_SOURCES_DIR "D:/C++/Projects/PathTracer/EngineRuntime/AssetSources"
#define RESOURCES_DIR "D:/C++/Projects/PathTracer/EngineRuntime/Resources"

/// <summary>
/// 不允许实例化，只能实例化其子类
/// </summary>
class ResourceBase {
public:
	Uuid uuid;
	std::vector<Uuid> references;

	virtual const std::string& GetResourceType() const = 0;

	static std::string Serialize(const ResourceBase& data);
	static std::unique_ptr<ResourceBase> Deserialize(const std::string& str);
	virtual ~ResourceBase() = default;
};

template <typename TData>
class Resource : public ResourceBase {
public:
	TData resource_data;

	static const std::string& GetResourceTypeStatic();

	const std::string& GetResourceType() const override {
		return Resource<TData>::GetResourceTypeStatic();
	}
};
