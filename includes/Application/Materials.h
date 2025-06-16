#pragma once

#include <glm/glm.hpp>
#include "Engine/Guid.h"
#include "Engine/Resources/Resources.h"
#include "Engine/Resources/ResourceData.h"

#define ASTAR_GET_STRING_X(x) #x
#define ASTAR_REGISTER_MATERIAL_NAME(type) template <> constexpr ResourceTypeDisplay Resource<MaterialResourceData<##type##MaterialData>>::type_display = ASTAR_GET_STRING_X(Material##type)

struct SimpleLitMaterialData {
	Uuid diffuse_texture;
	glm::vec4 diffuse_color;
};

ASTAR_REGISTER_MATERIAL_NAME(SimpleLit);

struct SimpleLitMaterialRuntimeData {
	uint32_t diffuse_texture;
	glm::vec4 diffuse_color;
};

struct LightMaterialData {
	glm::vec3 color;
	float intensity;
};
ASTAR_REGISTER_MATERIAL_NAME(Light);

struct PureReflectionMaterialData {
	glm::vec4 color;
};
ASTAR_REGISTER_MATERIAL_NAME(PureReflection);

struct DielectricMaterialData {
	float eta;
};
ASTAR_REGISTER_MATERIAL_NAME(Dielectric);

#undef ASTAR_REGISTER_MATERIAL_NAME
#undef ASTAR_GET_STRING_X