#pragma once

#include <Engine/Guid.h>

struct MeshComponent {
public:
	Uuid id;
	Uuid resource;

	MeshComponent(const Uuid& id, const Uuid& resrouce) : id(id), resource(resrouce) {}
	MeshComponent(const Uuid& resource) : MeshComponent(xg::newGuid(), resource) {}
};

struct StaticMeshTag {};

struct DeformableMeshTag {};

struct MaterialComponent {
	Uuid id;
};