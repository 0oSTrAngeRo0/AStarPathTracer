#pragma once

#include <Engine/Guid.h>
#include <vector>
#include <entt/entt.hpp>

using ResourceTypeId = std::uint32_t;
using ResourceTypeDisplay = std::string_view;

/// <summary>
/// “Cannot be instantiated directly; only subclasses may be instantiated.”
/// </summary>
class ResourceBase {
  public:
	Uuid uuid;
	std::vector<Uuid> references;
	ResourceBase() : uuid(xg::newGuid()) {}
	virtual const ResourceTypeDisplay& GetResourceTypeDisplay() const = 0;
	virtual const ResourceTypeId GetResourceTypeId() const = 0;
	static std::string Serialize(const ResourceBase& data);
	static std::unique_ptr<ResourceBase> Deserialize(const std::string& str);
	virtual ~ResourceBase() = default;
};

template <typename TData>
class Resource : public ResourceBase {
  public:
	static constexpr ResourceTypeId type_id = entt::type_hash<Resource<TData>>::value();
	static constexpr ResourceTypeDisplay type_display = "";
	const ResourceTypeDisplay& GetResourceTypeDisplay() const override { return type_display; }
	const ResourceTypeId GetResourceTypeId() const override { return type_id; }

	TData resource_data;
};
