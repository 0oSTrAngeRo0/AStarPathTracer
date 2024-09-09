#version 460
#extension GL_GOOGLE_include_directive : enable

#include "../Includes/Common.glsl"
#include "../Includes/Math.glsl"
#include "../Includes/RayClosestHitCommon.glsl"

struct PureReflectionMaterial {
	vec4 color;
};

RAY_CLOSEST_HIT_DESCRIPTORS();
RAY_CLOSEST_HIT_MATERIAL_DESCRIPTOR(PureReflectionMaterial);
RAY_CLOSEST_HIT_RAY_TRACING_VARIABLES();

void main() {

	RAY_CLOSEST_HIT_RAY_TRACING_FETCH_DATA(instance, barycentric, index, vertex, PureReflectionMaterial, material);

	payload.position = vertex.position;
	payload.normal = vertex.normal;
	payload.is_stopped = false;
	payload.emittence = vec3(0);
	payload.throughput = vec3(1);
	payload.next_ray_direction = reflect(gl_WorldRayDirectionEXT, vertex.normal);
}
