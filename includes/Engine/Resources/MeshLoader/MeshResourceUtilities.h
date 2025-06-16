#pragma once

#include <glm/glm.hpp>
#include "Engine/StaticRegistry.h"
#include "Engine/Guid.h"
#include "Utilities/MacroUtilities.h"
#include "Engine/Resources/Resources.h"
#include "Engine/Resources/ResourcesManager.h"

class MeshData {
public:
	std::vector<glm::vec3> positions; // vertex positions
	std::vector<glm::vec3> normals; // per vertex normal
	std::vector<glm::vec4> tangents; // per vertex tangent, glm::vec3 bitangent = cross(normal, tangent.xyz) * tangent.w
	std::vector<glm::vec2> uvs; // per vertex uv
	std::vector<glm::uvec3> indices; // triangles
};

class MeshResourceUtilities : public StaticFunctionRegistry<ResourceTypeId, std::vector<MeshData>(const ResourceBase&)> {
public:
	using Base = StaticFunctionRegistry<ResourceTypeId, std::vector<MeshData>(const ResourceBase&)>;
	static std::vector<MeshData> Load(Uuid id) {
		auto& resource = ResourcesManager::GetInstance().GetResource(id);
		auto type_id = resource.GetResourceTypeId();
		return Call(type_id, resource);
	}
	template <typename T> static std::vector<MeshData> Load(const Resource<T>& resource);
	template <typename T> static void Register() {
		Base::Register(Resource<T>::type_id, [](const ResourceBase& resource) {
			return Load(static_cast<const Resource<T>&>(resource));
		});
	}
};
