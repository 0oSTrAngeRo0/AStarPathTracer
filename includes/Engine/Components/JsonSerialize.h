#pragma once

#include <entt/entt.hpp>
#include "Utilities/ReflectionX.h"
#include "Utilities/JsonX.h"
#include "Utilities/MacroUtilities.h"

namespace reflection {
	template <typename T, bool EnableTypeName = false>
	nlohmann::json JsonSerialize(const entt::registry& registry, entt::entity entity) {
		nlohmann::json json = registry.get<T>(entity);
		auto type = entt::resolve<T>();
		json.emplace("type_id", type.id());
		if constexpr (EnableTypeName) {
			json.emplace("type_name", GetTypeName(type.id()).value_or(std::string(type.info().name())));
		}
		return json;
	}

	template <typename T, bool EnableTypeName = false>
	inline void RegisterJsonSerialize() {
		entt::meta_factory<T>{}.template func<&JsonSerialize<T, EnableTypeName>>(entt::hashed_string("JsonSerialize"));
	}

	inline std::optional<nlohmann::json> JsonSerialize(entt::id_type id, const entt::registry& registry, entt::entity entity) {
		return CallFunc<nlohmann::json, const entt::registry&, entt::entity>(id, entt::hashed_string("JsonSerialize"), registry, entity);
	}

	template <typename T>
	inline bool JsonDeserialize(const nlohmann::json& json, entt::registry& registry, entt::entity entity) {
		T&& component = json.get<T>();
		registry.emplace<T>(entity, std::move(component));
		return true;
	}

	template <typename T>
	inline void RegisterJsonDeserialize() {
		entt::meta_factory<T>{}.template func<&JsonDeserialize<T>>(entt::hashed_string("JsonDeserialize"));
	}

	inline void JsonDeserialize(const nlohmann::json& json, entt::registry& registry, entt::entity entity) {
		entt::id_type type_id;
		json.at("type_id").get_to(type_id);
		CallFunc<void, const nlohmann::json&, entt::registry&, entt::entity>(type_id, entt::hashed_string("JsonDeserialize"), json, registry, entity);
	}
}

struct SceneResourceData {
public:
	struct EntityData {
	public:
		entt::entity entity;
		std::vector<nlohmann::json> components;
		EntityData() = default;
		
		EntityData(entt::entity entity, std::vector<nlohmann::json>&& components) :
			entity(entity), components(std::move(components)) {}
	};

	std::vector<EntityData> entities;
};

JSON_SERIALIZER(SceneResourceData::EntityData, <>, entity, components);
JSON_SERIALIZER(SceneResourceData, <>, entities);

inline std::string SerializeRegistry(const entt::registry& registry) {
	std::unordered_map<entt::entity, std::vector<nlohmann::json>> scene;
	for (auto [id, storage] : registry.storage()) {
		for (auto entity : storage) {
			auto res_opt = reflection::JsonSerialize(id, registry, entity);
			if (!res_opt) continue;
			scene[entity].emplace_back(res_opt.value());
		}
	}
	SceneResourceData scene_resource;
	for (auto&& [entity, components] : scene) {
		SceneResourceData::EntityData entity_data(entity, std::move(components));
		scene_resource.entities.emplace_back(std::move(entity_data));
	}
	nlohmann::json json = scene_resource;
	return json.dump(4);
}

inline void DeserializeRegistry(const std::string& str, entt::registry& registry) {
	nlohmann::json json = nlohmann::json::parse(str);
	SceneResourceData scene = json.template get<SceneResourceData>();
	for (auto& entity_data : scene.entities) {
		entt::entity entity = registry.create(entity_data.entity);
		for (auto& component : entity_data.components) {
			reflection::JsonDeserialize(component, registry, entity);
		}
	}
}