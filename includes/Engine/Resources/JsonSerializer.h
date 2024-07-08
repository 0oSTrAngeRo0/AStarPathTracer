#pragma once

#include <nlohmann/json.hpp>
#include "Engine/Resources/Resources.h"

NLOHMANN_JSON_SERIALIZE_ENUM(ResourceType, {
	{eObj, "Obj"},
	{eMaterial, "Material"}
	})

#define NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_STRUCT(Type, Generic, ...)  \
template Generic \
struct adl_serializer<Type> { \
	static inline void to_json(nlohmann::json& nlohmann_json_j, const Type& nlohmann_json_t) { NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_TO, __VA_ARGS__)) } \
    static inline void from_json(const nlohmann::json& nlohmann_json_j, Type& nlohmann_json_t) { NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_FROM, __VA_ARGS__)) } \
};

	namespace nlohmann {
	template <>
	struct adl_serializer<xg::Guid> {
		static void to_json(json& j, const xg::Guid& obj) {
			if (!obj.isValid()) {
				j = nullptr;
				return;
			}
			j = obj.str();
		}
		static void from_json(const json& j, xg::Guid& obj) {
			if (j.is_null()) {
				obj = xg::Guid();
				return;
			}
			obj = xg::Guid(j.template get<std::string>());
		}
	};

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_STRUCT(ObjResourceData, <>, path)

		NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_STRUCT(MaterialResourceData, <>, material_type)

		NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_STRUCT(ResourceTemplate<TData>, <typename TData>, resource_type, uuid, references, resource_data)

		template <>
	struct adl_serializer<ResourceBase> {
		static void to_json(json& j, const ResourceBase& obj) {
			if (!obj.uuid.isValid()) {
				j = nullptr;
				return;
			}
			if (obj.resource_type == ResourceType::eObj) j = static_cast<const ResourceTemplate<ObjResourceData>&>(obj);
			else if (obj.resource_type == ResourceType::eMaterial) j = static_cast<const ResourceTemplate<MaterialResourceData>&>(obj);
			else j = nullptr;
		}
		static std::optional<std::unique_ptr<ResourceBase>> from_json(const json& j) {
			if (j.is_null()) {
				return std::nullopt;
			}
			ResourceType type;
			j.at("resource_type").get_to(type);
			if (type == ResourceType::eObj) { 
				return std::make_unique<ResourceTemplate<ObjResourceData>>(j.template get<ResourceTemplate<ObjResourceData>>());
			}
			else if (type == ResourceType::eMaterial) { 
				return std::make_unique<ResourceTemplate<MaterialResourceData>>(j.template get<ResourceTemplate<MaterialResourceData>>()); 
			}
			else return std::nullopt;
		}
	};
}