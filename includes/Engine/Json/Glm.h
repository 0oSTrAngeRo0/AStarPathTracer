#pragma once 

#include "Utilities/JsonX.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <span>

JSON_SERIALIZER(glm::vec3, <>, x, y, z);
JSON_SERIALIZER(glm::vec4, <>, x, y, z, w);
JSON_SERIALIZER(glm::quat, <>, x, y, z, w);

namespace nlohmann {
	template <>  struct adl_serializer<glm::mat4> {
		static constexpr glm::length_t Size = glm::mat4::col_type::length() * glm::mat4::row_type::length();
		using TElem = glm::mat4::value_type;
		using TArray = std::array<TElem, Size>;
		static void to_json(json& j, const glm::mat4& obj) {
			const TArray& array = *reinterpret_cast<const TArray*>(glm::value_ptr(obj));
			j = array;
		}
		static void from_json(const json& j, glm::mat4& obj) {
			if (j.is_null()) {
				return;
			}
			obj = std::move(glm::make_mat4(j.get<TArray>().data()));
		}
	};
}