#pragma once

#include <entt/entt.hpp>

class InputState;

class World {
public:
	World(const InputState& input);
	void Update(float delta_time);
	inline entt::registry& GetRegistry() { return registry; }
	~World();
private:
	entt::registry registry;

	// World Creations
	static entt::entity CreateCamera(entt::registry& registry);
	static entt::entity CreateCube(entt::registry& registry);
	static void CreateDefault(entt::registry& registry);

	// Systems
	static void UpdateLinearVelocity(entt::registry & registry, float delta_time);
	static void UpdateLocalTransform(entt::registry & registry);
	static void UpdateOrbitCamera(entt::registry& registry);
	static void UpdateProjectiveCamera(entt::registry & registry);
};