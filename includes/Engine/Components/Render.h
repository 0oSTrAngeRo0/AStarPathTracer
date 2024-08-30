#pragma once

#include <Engine/Guid.h>

struct MeshComponent {
public:
	std::vector<Uuid> device_ids;
	Uuid resource_id;

	MeshComponent(const MeshComponent& other) = default;
	MeshComponent(MeshComponent&& other) = default;

	MeshComponent(const std::vector<Uuid>& device_ids, const Uuid& resource_id) : device_ids(device_ids), resource_id(resource_id) {}
};

struct StaticMeshTag {};

struct DeformableMeshTag {};

struct MaterialComponent {
	std::vector<Uuid> resource_ids;

	MaterialComponent(const std::vector<Uuid>& resource_ids) : resource_ids(resource_ids) {}
};