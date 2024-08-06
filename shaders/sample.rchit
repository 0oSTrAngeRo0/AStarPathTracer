#version 460
#extension GL_EXT_scalar_block_layout : enable
#extension GL_GOOGLE_include_directive : enable
#extension GL_EXT_shader_explicit_arithmetic_types_int32 : require
#extension GL_EXT_buffer_reference2 : require
#extension GL_EXT_ray_tracing : require
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require
#extension GL_EXT_shader_explicit_arithmetic_types_int16 : require

#include "Common.glsl"
#include "Math.glsl"

layout(set = 0, binding = 2, buffer_reference, std430) readonly buffer Vertices { Vertex data[]; };
layout(set = 0, binding = 3, buffer_reference, std430) readonly buffer Indices { u32vec3 data[];};
layout(set = 0, binding = 4, buffer_reference, std430) readonly buffer Materials { LitMaterial data[]; };
layout(set = 0, binding = 5, buffer_reference, std430) readonly buffer Instances { InstanceData data[]; } instances;

layout(location = 0) rayPayloadInEXT HitPayload payload;
hitAttributeEXT vec3 attribs;

struct FetchedVertex {
	vec3 position;
	vec3 normal;
	vec3 tangent;
	vec3 bitangent;
	vec2 uv;
};

void main() {
	InstanceData instance = instances.data[gl_InstanceCustomIndexEXT];

	vec3 barycentric = vec3(1.0 - attribs.x - attribs.y, attribs.x, attribs.y);
	u32vec3 index = Indices(instance.index_address).data[gl_PrimitiveID];
	uint64_t address = instance.vertex_address;
	Vertex v0 = Vertices(address).data[index.x];
	Vertex v1 = Vertices(address).data[index.y];
	Vertex v2 = Vertices(address).data[index.z];
	FetchedVertex v;
	v.position = INTERPOLATE_BARYCENTRIC(v0, v1, v2, barycentric, VERTEX_GET_POSITION);
	v.normal = INTERPOLATE_BARYCENTRIC(v0, v1, v2, barycentric, VERTEX_GET_NORMAL);
	v.tangent = INTERPOLATE_BARYCENTRIC(v0, v1, v2, barycentric, VERTEX_GET_TANGENT);
	v.bitangent = INTERPOLATE_BARYCENTRIC(v0, v1, v2, barycentric, VERTEX_GET_BITANGENT);
	v.uv = INTERPOLATE_BARYCENTRIC(v0, v1, v2, barycentric, VERTEX_GET_UV);

	LitMaterial material = Materials(instance.material_address).data[instance.material_index];
	payload.color = vec4(v.position.xyz, 1.0);
}
