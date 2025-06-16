#pragma once

#include <nlohmann/json.hpp>
#include <glm/glm.hpp>
#include "Engine/Resources/Resources.h"
#include "Engine/Resources/ResourceRegistry.h"
#include "Utilities/JsonX.h"
#include "Engine/Json/Guid.h"
#include <vulkan/vulkan.hpp>

namespace nlohmann {
	template <typename TData> struct adl_serializer<Resource<TData>> {
		static inline void to_json(json& nlohmann_json_j, const Resource<TData>& nlohmann_json_t) {
			nlohmann_json_j["uuid"] = nlohmann_json_t.uuid;
			nlohmann_json_j["references"] = nlohmann_json_t.references;
			nlohmann_json_j["resource_data"] = nlohmann_json_t.resource_data;
			nlohmann_json_j["resource_type"] = Resource<TData>::type_display;
		}
		static inline void from_json(const json& nlohmann_json_j, Resource<TData>& nlohmann_json_t) {
			nlohmann_json_j.at("uuid").get_to(nlohmann_json_t.uuid);
			nlohmann_json_j.at("references").get_to(nlohmann_json_t.references);
			nlohmann_json_j.at("resource_data").get_to(nlohmann_json_t.resource_data);
		}
	};
};

namespace nlohmann {
	template <>
	struct adl_serializer<ResourceBase> {
		static void to_json(json& j, const ResourceBase& obj) {
			if (!obj.uuid.isValid()) {
				throw std::runtime_error("Invalid uuid when serialize resource");
			}
			auto function = ResourceSerializeRegistry::Get(std::string(obj.GetResourceTypeDisplay()));
			if (!function.has_value()) {
				throw std::runtime_error("Invalid resource type when serialize resource");
			}
			function.value()(j, obj);
		}
		static std::optional<std::unique_ptr<ResourceBase>> from_json(const json& j) {
			if (j.is_null()) {
				return std::nullopt;
			}
			std::string type;
			j.at("resource_type").get_to(type);
			auto function = ResourceDeserializerRegistry::Get(type);
			if (!function.has_value()) {
				return std::nullopt;
			}
			return function.value()(j);
		}
	};
}