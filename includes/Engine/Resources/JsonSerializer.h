#include <nlohmann/json.hpp>
#include "Engine/Resources/Resources.h"

NLOHMANN_JSON_SERIALIZE_ENUM(ResourceType, {
	{ResourceType::eUnknown, "Unknown"},
	{ResourceType::eObj, "Obj"},
	{ResourceType::eMaterial, "Material"}
});

NLOHMANN_JSON_SERIALIZE_ENUM(MaterialType, {
	{MaterialType::eUnknown, "Unknown"},
	{MaterialType::eSimpleLit, "SimpleLit"}
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

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_STRUCT(glm::vec4, <>, x, y, z, w);

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_STRUCT(SimpleLitMaterialData, <>, color);

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_STRUCT(ObjResourceData, <>, path);

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_STRUCT(MaterialResourceData<TMatData>, <typename TMatData>, material_type, material_data);

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_STRUCT(ResourceTemplate<TData>, <typename TData>, resource_type, uuid, references, resource_data);


	std::unordered_map<MaterialType, std::function<void(json&, const ResourceBase&)>> material_serializers = {
		{
			MaterialType::eSimpleLit, [](json& j, const ResourceBase& obj) {
				j = static_cast<const ResourceTemplate<MaterialResourceData<SimpleLitMaterialData>>&>(obj);
			}
		}
	};

	std::unordered_map<MaterialType, std::function<std::optional<std::unique_ptr<ResourceBase>>(const json&)>> material_deserializers = {
		{
			MaterialType::eSimpleLit, [](const json& j) {
				return std::make_unique<ResourceTemplate<MaterialResourceData<SimpleLitMaterialData>>>(j.template get<ResourceTemplate<MaterialResourceData<SimpleLitMaterialData>>>());
			}
		}
	};

	std::unordered_map<ResourceType, std::function<void(json&, const ResourceBase&)>> serializers = {
		{
			ResourceType::eObj, [](json& j, const ResourceBase& obj) {
				j = static_cast<const ResourceTemplate<ObjResourceData>&>(obj);
			}
		},
		{
			ResourceType::eMaterial, [](json& j, const ResourceBase& obj) {
				auto material_obj = static_cast<const ResourceTemplate<MaterialResourceDataBase>&>(obj);
				auto iter = material_serializers.find(material_obj.resource_data.material_type);
				if (iter != material_serializers.end()) {
					iter->second(j, obj);
				}
				else j = nullptr;
			}
		}
	};

	std::unordered_map<ResourceType, std::function<std::optional<std::unique_ptr<ResourceBase>>(const json&)>> deserializers = {
		{
			ResourceType::eObj, [](const json& j) {
				return std::make_unique<ResourceTemplate<ObjResourceData>>(j.template get<ResourceTemplate<ObjResourceData>>());
			}
		},
		{
			ResourceType::eMaterial, [](const json& j) -> std::optional<std::unique_ptr<ResourceBase>> {
				MaterialType material_type;
				j.at("resource_data").at("material_type").get_to(material_type);
				auto iter = material_deserializers.find(material_type);
				if (iter != material_deserializers.end()) {
					return iter->second(j);
				}
				else return std::nullopt;
			}
		}
	};

	template <>
	struct adl_serializer<ResourceBase> {
		static void to_json(json& j, const ResourceBase& obj) {
			if (!obj.uuid.isValid()) {
				j = nullptr;
				return;
			}
			auto iter = serializers.find(obj.resource_type);
			if (iter != serializers.end()) {
				iter->second(j, obj);
			}
			else j = nullptr;
		}
		static std::optional<std::unique_ptr<ResourceBase>> from_json(const json& j) {
			if (j.is_null()) {
				return std::nullopt;
			}
			ResourceType type;
			j.at("resource_type").get_to(type);
			auto iter = deserializers.find(type);
			if (iter != deserializers.end()) {
				return iter->second(j);
			}
			else return std::nullopt;
		}
	};
}