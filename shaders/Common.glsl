#ifndef SHADER_COMMON_GLSL
#define SHADER_COMMON_GLSL

#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require
#extension GL_EXT_shader_explicit_arithmetic_types_int16 : require
#extension GL_EXT_ray_tracing : enable

struct Vertex {
	vec3 position;
};

struct LitMaterial {
	vec4 color;
};

struct InstanceData {
	uint64_t vertex_address;
	uint64_t index_address;
	uint64_t material_address;
	uint16_t material_index;
};

struct HitPayload {
	vec4 color;
};

#endif