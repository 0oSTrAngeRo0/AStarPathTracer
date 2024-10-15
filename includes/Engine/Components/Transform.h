#pragma once

#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

struct LocalLinearVelocity {
	glm::vec3 velocity;

	LocalLinearVelocity() = default;
	LocalLinearVelocity(glm::vec3 velocity) : velocity(velocity) {}
};

struct LocalPosition {
	glm::vec3 position;

	LocalPosition() = default;
	LocalPosition(glm::vec3 position) :position(position) {}
	operator const glm::vec3& () const { return position; }
};

struct LocalRotation {
	glm::quat rotation;
};

struct LocalScale {
	glm::vec3 scale;
};

struct LocalTransformDirty {};

struct LocalTransform {
	glm::mat4x4 matrix;
};

struct Relationship {
	entt::entity first; // first child
	entt::entity parent; // parent
	entt::entity next; // next sibling
	entt::entity previous; // previous sibling
};