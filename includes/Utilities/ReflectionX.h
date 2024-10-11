#pragma once

#include <entt/entt.hpp>
#include <optional>

namespace reflection {
	template <typename T>
	inline void RegisterTypeName(const std::string name) {
		entt::meta<T>().prop(entt::hashed_string("TypeName"), name);
	}

	inline std::optional<std::string> GetTypeName(entt::id_type id) {
		auto&& prop = entt::resolve(id).prop(entt::hashed_string("TypeName"));
		if (prop) return prop.value().cast<std::string>();
		else return std::nullopt;
	}
}