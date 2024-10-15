#pragma once

#include <nlohmann/json.hpp>

#define JSON_SERIALIZER(Type, Generic, ...)  \
namespace nlohmann { \
	template Generic \
	struct adl_serializer<Type> { \
		static inline void to_json(json& nlohmann_json_j, const Type& nlohmann_json_t) { \
			NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_TO, __VA_ARGS__)) \
		} \
		static inline void from_json(const json& nlohmann_json_j, Type& nlohmann_json_t) { \
			NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_FROM, __VA_ARGS__)) \
		} \
	}; \
}

#define JSON_ENUM_SERIALIZER(ENUM_TYPE, ...) \
namespace nlohmann { \
	NLOHMANN_JSON_SERIALIZE_ENUM(ENUM_TYPE, __VA_ARGS__) \
}