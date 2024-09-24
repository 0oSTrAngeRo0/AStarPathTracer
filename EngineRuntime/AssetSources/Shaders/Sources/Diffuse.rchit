#version 460
#extension GL_GOOGLE_include_directive : enable

#include "../Includes/Common.glsl"
#include "../Includes/Math.glsl"
#include "../Includes/RayClosestHitCommon.glsl"

struct SimpleLitMaterial {
	uint32_t diffuse_texture;
	vec4 diffuse_color;
};

RAY_CLOSEST_HIT_DESCRIPTORS();
RAY_CLOSEST_HIT_MATERIAL_DESCRIPTOR(SimpleLitMaterial);
RAY_CLOSEST_HIT_RAY_TRACING_VARIABLES();

void main() {

	RAY_CLOSEST_HIT_RAY_TRACING_FETCH_DATA(instance, barycentric, index, vertex, SimpleLitMaterial, material);

	payload.position = vertex.position;
	payload.normal = vertex.normal;
	payload.is_stopped = false;
	payload.emittence = vec3(0);

	// throughput = brdf * cos(n, omega_input) / pdf(omega_input)
	// brdf of lamertian: brdf = albedo / PI
	// sampling of omege_input is uniform sampling, pdf(omega_input) = 1 / (2 * PI)
	// further reduction, throughput = 2 * albedo * cos;
	// if use cosine weighted sampling, then pdf(omega_input) = cosine(omega_input) / PI, throughput = albedo
	vec3 ray_direction = HemiSphereSampleCosineWeighted(payload.random_seed);
	ray_direction = mat3x3(vertex.tangent, vertex.bitangent, vertex.normal) * ray_direction;
	float cosine_theta = dot(vertex.normal, ray_direction);

	vec3 diffuse = material.diffuse_color.xyz;
	if (material.diffuse_texture != INVALID_TEXTURE_ID) {
		diffuse *= texture(textures2D[material.diffuse_texture], vertex.uv).xyz;
	}

	vec3 throughput = diffuse; 
	payload.throughput = throughput;
	payload.next_ray_direction = ray_direction;
}
