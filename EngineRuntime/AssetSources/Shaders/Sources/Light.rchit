#version 460
#extension GL_GOOGLE_include_directive : enable

#include "../Includes/Common.glsl"
#include "../Includes/Math.glsl"
#include "../Includes/RayClosestHitCommon.glsl"

struct LightMaterial {
	vec3 color;
	float intensity;
};

RAY_CLOSEST_HIT_DESCRIPTORS();
RAY_CLOSEST_HIT_MATERIAL_DESCRIPTOR(LightMaterial);
RAY_CLOSEST_HIT_RAY_TRACING_VARIABLES();

void main() {
	RAY_CLOSEST_HIT_RAY_TRACING_FETCH_DATA(instance, barycentric, index, vertex, LightMaterial, material);

	payload.emittence = material.color.xyz * material.intensity;
	payload.throughput = vec3(1);
	payload.is_stopped = true;
}
