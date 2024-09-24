#pragma once

#include <glm/glm.hpp>
#include "Engine/Guid.h"

struct SimpleLitMaterialData {
	Uuid diffuse_texture;
	glm::vec4 diffuse_color;
};

struct SimpleLitMaterialRuntimeData {
	uint32_t diffuse_texture;
	glm::vec4 diffuse_color;
};

struct LightMaterialData {
	glm::vec3 color;
	float intensity;
};

struct PureReflectionMaterialData {
	glm::vec4 color;
};

struct DielectricMaterialData {
	float eta;
};