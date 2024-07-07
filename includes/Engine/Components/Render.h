#pragma once

#include <memory>
#include "Core/Mesh.h"
#include "Core/Shader.h"
#include "Core/Instance.h"

struct Renderable {
	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<Material> material;
	std::shared_ptr<InstanceHandler> instance;
};