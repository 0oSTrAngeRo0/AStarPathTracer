#pragma once

#include <Engine/Guid.h>
#include <vector>

using ResourceType = std::string;

/// <summary>
/// 不允许实例化，只能实例化其子类
/// </summary>
class ResourceBase {
public:
	Uuid uuid;
	std::vector<Uuid> references;
	ResourceBase() : uuid(xg::newGuid()) {}

	virtual const ResourceType& GetResourceType() const = 0;
	static std::string Serialize(const ResourceBase& data);
	static std::unique_ptr<ResourceBase> Deserialize(const std::string& str);
	virtual ~ResourceBase() = default;
};

template <typename TData>
class Resource : public ResourceBase {
public:
	TData resource_data;

	static const ResourceType& GetResourceTypeStatic();

	const ResourceType& GetResourceType() const override {
		return Resource<TData>::GetResourceTypeStatic();
	}
};
