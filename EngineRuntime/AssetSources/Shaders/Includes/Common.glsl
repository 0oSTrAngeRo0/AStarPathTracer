#ifndef SHADER_COMMON_GLSL
#define SHADER_COMMON_GLSL

#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require
#extension GL_EXT_shader_explicit_arithmetic_types_int16 : require
#extension GL_EXT_ray_tracing : enable
#extension GL_EXT_shader_explicit_arithmetic_types_int32 : require


struct HitPayload {
	vec3 next_ray_direction;
	vec3 normal; // hit world normal
	vec3 position; // hit world position
	vec3 throughput;
	vec3 emittence;
	float eta;
	uint32_t random_seed; // seed for generate random number
	bool is_stopped;
};

struct ConstantsData {
	mat4 view_inverse;
	mat4 projection_inverse;
	uint32_t samples_per_pixel;
};

#endif