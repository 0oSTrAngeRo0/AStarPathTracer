#pragma once

#include <glm/glm.hpp>

struct ProjectionCamera {
	float near_z;
	float far_z;
	float aspect;
	float fov_y;

	ProjectionCamera(float fov_y, float aspect, float near_z, float far_z) :
		fov_y(fov_y), aspect(aspect), near_z(near_z), far_z(far_z) {}
};

struct Camera {
	glm::mat4 view;
	glm::mat4 projection;
};