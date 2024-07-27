#pragma once

#include "Engine/StaticRegistry.h"
#include <string>
#include <functional>
#include <nlohmann/json.hpp>

class ResourceBase;

class ResourceSerializeRegistry : public StaticRegistry<std::string, std::function<void(nlohmann::json&, const ResourceBase&)>> {};
class ResourceDeserializerRegistry : public StaticRegistry<std::string, std::function<std::optional<std::unique_ptr<ResourceBase>>(const nlohmann::json&)>> {};

#define REGISTER_RESOURCE_SERIALIZER(name, type) \
namespace nlohmann { \
	static bool resource_serialize_register_##name = (ResourceSerializeRegistry::Register(#name, \
		[](json& j, const ResourceBase& obj) { \
			j = static_cast<const Resource<type>&>(obj); \
		}), true); \
}


#define REGISTER_RESOURCE_DESERIALIZER(name, type) \
namespace nlohmann { \
	static bool resource_deserialize_register_##name = (ResourceDeserializerRegistry::Register(#name, \
		[](const nlohmann::json& j) { \
			return std::make_unique<Resource<type>>(j.template get<Resource<type>>()); \
		}), true); \
} 
