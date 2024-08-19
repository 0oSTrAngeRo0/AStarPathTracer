#pragma once

#include <entt/entt.hpp>

class World {
public:
	static void Update(entt::registry& registry, float delta_time);
	static void CreateDefault(entt::registry& registry);
private:
	// World Creations
	static entt::entity CreateCamera(entt::registry& registry);
	static entt::entity CreateCube(entt::registry& registry);
	static entt::entity CreateLight(entt::registry& registry);

	// Systems
	static void UpdateMaterialsRegistry(entt::registry& registry);
	static void UpdateLinearVelocity(entt::registry & registry, float delta_time);
	static void UpdateLocalTransform(entt::registry & registry);
	static void UpdateOrbitCamera(entt::registry& registry);
	static void UpdateProjectiveCamera(entt::registry & registry);
};