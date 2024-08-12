#pragma once

#include "Engine/ShaderHostBuffer.h"
#include <string>
#include <glm/glm.hpp>

struct ObjResourceData {
public:
	std::string path; // relative path
	// other import settings
};

template <typename T>
struct MaterialResourceData {
private:
	std::shared_ptr<HostBufferVisitor<T>> material_visitor;
public:
	T material_data;
	MaterialResourceData();
};

struct SimpleLitMaterialData {
	glm::vec4 color;
};
