#pragma once

#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include "Engine/Components/Render.h"

class World {
public:
	static void Update(entt::registry& registry, float delta_time);
	static void CreateDefault(entt::registry& registry);
private:
	enum class MeshInstance {
		eCube,
		eCornellBox,
		eQuad,
	};

	static entt::entity CreateCamera(entt::registry& registry);
	static entt::entity CreateCube(entt::registry& registry);
	static entt::entity CreateLight(entt::registry& registry);
	static entt::entity AttachOrbitCamera(entt::registry& registry, entt::entity camera, glm::vec3 position, float distance);
	static void CreateCornellBox(entt::registry& registry);
	static void CreateAxises(entt::registry& registry, float length, float size);
	template <MeshInstance instance> static MeshComponent GetMeshInstance();

	// Systems
	static void UpdateMaterialsRegistry(entt::registry& registry);
	static void UpdateLinearVelocity(entt::registry & registry, float delta_time);
	static void UpdateLocalTransform(entt::registry & registry);
	static void UpdateOrbitCamera(entt::registry& registry);
	static void UpdateProjectiveCamera(entt::registry & registry);
};
