#version 460
#extension GL_GOOGLE_include_directive : enable

#include "../Includes/Common.glsl"
#include "../Includes/Math.glsl"
#include "../Includes/RayClosestHitCommon.glsl"

struct DielectricMaterial {
	float eta;
};

RAY_CLOSEST_HIT_DESCRIPTORS();
RAY_CLOSEST_HIT_MATERIAL_DESCRIPTOR(DielectricMaterial);
RAY_CLOSEST_HIT_RAY_TRACING_VARIABLES();

void main() {

	RAY_CLOSEST_HIT_RAY_TRACING_FETCH_DATA(instance, barycentric, index, vertex, DielectricMaterial, material);

	payload.position = vertex.position;
	payload.is_stopped = false;
	payload.emittence = vec3(0);
	payload.throughput = vec3(1);

	float cos_theta_i = dot(-gl_WorldRayDirectionEXT, vertex.normal);
	vec3 next_ray_direction;
	vec3 normal;
	float ior;
	float eta_t;
	if(cos_theta_i > 0) { // enter model
		ior = material.eta / payload.eta;
		normal = vertex.normal;
		eta_t = material.eta;
	}
	else { // exit model
		ior = 1.0 / material.eta;
		normal = -vertex.normal;
		cos_theta_i *= -1;
		eta_t = 1.0;
	}

	vec3 refract_dir = refract(gl_WorldRayDirectionEXT, normal, ior);
	float cos_theta_t = dot(-normal, refract_dir);
	float reflectance = Fresnel(cos_theta_i, cos_theta_t, ior);

	if(Rnd(payload.random_seed) < reflectance) { // reflect
		next_ray_direction = reflect(gl_WorldRayDirectionEXT, vertex.normal);
	}
	else { // refract
		next_ray_direction = refract_dir;
		payload.eta = eta_t;
		normal *= -1;
	}

	payload.normal = normal;
	payload.next_ray_direction = next_ray_direction;
}
