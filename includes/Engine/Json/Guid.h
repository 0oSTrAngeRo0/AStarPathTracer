#pragma once 

#include "Utilities/JsonX.h"
#include "Engine/Guid.h"

namespace nlohmann {
	template <> struct adl_serializer<xg::Guid> {
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
			obj = xg::Guid(j.get<std::string>());
		}
	};
}