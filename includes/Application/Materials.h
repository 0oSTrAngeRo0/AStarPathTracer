#pragma once

#include <glm/glm.hpp>

struct SimpleLitMaterialData {
	glm::vec4 color;
};

struct LitghtMaterialData {
	glm::vec3 color;
	float intensity;
};

struct PureReflectionMaterialData {
	glm::vec4 color;
};