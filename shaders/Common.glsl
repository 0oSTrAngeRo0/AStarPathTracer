#ifndef SHADER_COMMON_GLSL
#define SHADER_COMMON_GLSL

#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require
#extension GL_EXT_shader_explicit_arithmetic_types_int16 : require
#extension GL_EXT_ray_tracing : enable

#define DO_NOTHING(v) v
#define VERTEX_GET_NORMAL(v) (v.normal)
#define VERTEX_GET_TANGENT(v) (v.tangent.xyz)
#define VERTEX_GET_BITANGENT(v) (cross(v.normal, v.tangent.xyz) * v.tangent.w)
#define VERTEX_GET_UV(v) (v.uv)

struct VertexOther {
	vec3 normal;
	vec4 tangent;
	vec2 uv;
};

struct LitMaterial {
	vec4 color;
};

struct InstanceData {
	uint64_t vertex_position_address;
	uint64_t vertex_other_address;
	uint64_t index_address;
	uint64_t material_address;
	uint16_t material_index;
};

struct HitPayload {
	vec4 color;
};

struct ConstantsData {
	mat4 view_inverse;
	mat4 projection_inverse;
};

#endif