#pragma once

#include <glm/glm.hpp>
#include <entt/entt.hpp>

struct ProjectionCamera {
	float near_z;
	float far_z;
	float aspect;
	float fov_y_degree;

	ProjectionCamera(float fov_y_degree, float aspect, float near_z, float far_z) :
		fov_y_degree(fov_y_degree), aspect(aspect), near_z(near_z), far_z(far_z) {}
};

struct Camera {
	glm::mat4 view;
	glm::mat4 projection;
};

struct OrbitCamera {
	entt::entity look_at;
	float distance;
	float theta;
	float phi;

	OrbitCamera(entt::entity look_at, float distance, float theta, float phi) :
		look_at(look_at), distance(distance), theta(theta), phi(phi) {}
};

struct CameraDirtyTag {};