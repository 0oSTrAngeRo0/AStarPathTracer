#pragma once

#include "Engine/StaticRegistry.h"
#include <string>
#include <functional>
#include <nlohmann/json.hpp>
#include "Utilities/MacroUtilities.h"

class ResourceBase;

class ResourceSerializeRegistry : public StaticRegistry<std::string, std::function<void(nlohmann::json&, const ResourceBase&)>> {};
class ResourceDeserializerRegistry : public StaticRegistry<std::string, std::function<std::optional<std::unique_ptr<ResourceBase>>(const nlohmann::json&)>> {};

#define REGISTER_RESOURCE_SERIALIZER(type) \
namespace nlohmann { \
	static bool ASTAR_UNIQUE_VARIABLE_NAME(resource_serialize_register_) = (ResourceSerializeRegistry::Register(Resource<type>::GetResourceTypeStatic(), \
		[](json& j, const ResourceBase& obj) { \
			j = static_cast<const Resource<type>&>(obj); \
		}), true); \
}


#define REGISTER_RESOURCE_DESERIALIZER(type) \
namespace nlohmann { \
	static bool ASTAR_UNIQUE_VARIABLE_NAME(resource_deserialize_register_) = (ResourceDeserializerRegistry::Register(Resource<type>::GetResourceTypeStatic(), \
		[](const nlohmann::json& j) { \
			return std::make_unique<Resource<type>>(j.template get<Resource<type>>()); \
		}), true); \
} 
