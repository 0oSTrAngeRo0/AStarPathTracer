#pragma once

#include <glm/glm.hpp>

#include "Engine/StaticRegistry.h"
#include "Engine/Guid.h"

class ResourceBase;
template <typename T> class Resource;

class MeshData {
public:
	std::vector<glm::vec3> positions; // vertex positions
	std::vector<glm::vec3> normals; // per vertex normal
	std::vector<glm::vec4> tangents; // per vertex tangent, glm::vec3 bitangent = cross(normal, tangent.xyz) * tangent.w
	std::vector<glm::vec2> uvs; // per vertex uv
	std::vector<glm::uvec3> indices; // triangles
};

class MeshResourceUtilities : public StaticRegistry<std::string, std::function<MeshData(const ResourceBase&)>> {
public:
	static MeshData Load(Uuid id);
	template <typename T> static MeshData Load(const Resource<T>& resource);
};

#define REGISTER_RESOURCE_MESH_LOADER(name, type) \
	static bool register_resource_mesh_loader_##name = (MeshResourceUtilities::Register(#name, \
		[](const ResourceBase& resource) { \
			return MeshResourceUtilities::Load(static_cast<const Resource<type>&>(resource)); \
		}), true) \
