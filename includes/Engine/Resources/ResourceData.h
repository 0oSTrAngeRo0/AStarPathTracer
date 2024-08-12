#pragma once

#include <string>
#include <glm/glm.hpp>
#include "Engine/Guid.h"

struct ObjResourceData {
public:
	std::string path; // relative path
	// other import settings
};

template <typename T>
struct MaterialResourceData {
public:
	T material_data;
	Uuid runtime_host_id;
	MaterialResourceData();
	const Uuid GetShaderId() const;
	inline const Uuid GetMaterialId() const { return runtime_host_id; }
	const size_t GetMaterialIndex() const;
};

struct SimpleLitMaterialData {
	glm::vec4 color;
};
