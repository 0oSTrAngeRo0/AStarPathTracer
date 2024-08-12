#pragma once

#include <Engine/Guid.h>

struct MeshComponent {
public:
	Uuid device_id;
	Uuid resource_id;

	MeshComponent(const Uuid& device_id, const Uuid& resource_id) : device_id(device_id), resource_id(resource_id) {}
	MeshComponent(const Uuid& resource_id) : MeshComponent(xg::newGuid(), resource_id) {}
};

struct StaticMeshTag {};

struct DeformableMeshTag {};

struct MaterialComponent {
	Uuid resource_id;

	MaterialComponent(const Uuid& resource_id) : resource_id(resource_id) {}
};