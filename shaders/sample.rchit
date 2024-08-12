#version 460
#extension GL_EXT_scalar_block_layout : enable
#extension GL_GOOGLE_include_directive : enable
#extension GL_EXT_shader_explicit_arithmetic_types_int32 : require
#extension GL_EXT_buffer_reference2 : require
#extension GL_EXT_ray_tracing : require
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require
#extension GL_EXT_shader_explicit_arithmetic_types_int16 : require
#extension GL_EXT_debug_printf : enable

#include "Common.glsl"
#include "Math.glsl"

layout(set = 0, binding = 2, buffer_reference, scalar) readonly buffer VerticesPosition { vec3 data[]; };
layout(set = 0, binding = 3, buffer_reference, scalar) readonly buffer VerticesOther { VertexOther data[]; };
layout(set = 0, binding = 4, buffer_reference, scalar) readonly buffer Indices { uvec3 data[];};
layout(set = 0, binding = 5, buffer_reference, scalar) readonly buffer Materials { LitMaterial data[]; };
layout(set = 0, binding = 6, buffer_reference, scalar) readonly buffer Instances { InstanceData data[]; } instances;

layout(location = 0) rayPayloadInEXT HitPayload payload;
hitAttributeEXT vec2 attribs;

struct FetchedVertex {
	vec3 position;
	vec3 normal;
	vec3 tangent;
	vec3 bitangent;
	vec2 uv;
};

void PrintVertex(VertexOther v) {
	debugPrintfEXT("normal:[%v3f], tangent:[%v4f], uv:[%v2f]\n", v.normal, v.tangent, v.uv);
}

void main() {
	InstanceData instance = instances.data[gl_InstanceCustomIndexEXT];

	vec3 barycentric = vec3(1.0 - attribs.x - attribs.y, attribs.x, attribs.y);
	uvec3 index = Indices(instance.index_address).data[gl_PrimitiveID];

	uint64_t vertex_position_address = instance.vertex_position_address;
	vec3 p0 = VerticesPosition(vertex_position_address).data[index.x];
	vec3 p1 = VerticesPosition(vertex_position_address).data[index.y];
	vec3 p2 = VerticesPosition(vertex_position_address).data[index.z];
	
	uint64_t vertex_other_address = instance.vertex_other_address;
	VertexOther v0 = VerticesOther(vertex_other_address).data[index.x];
	VertexOther v1 = VerticesOther(vertex_other_address).data[index.y];
	VertexOther v2 = VerticesOther(vertex_other_address).data[index.z];

	FetchedVertex v;
	v.position = INTERPOLATE_BARYCENTRIC(p0, p1, p2, barycentric, DO_NOTHING);
	v.normal = INTERPOLATE_BARYCENTRIC(v0, v1, v2, barycentric, VERTEX_GET_NORMAL);
	v.tangent = INTERPOLATE_BARYCENTRIC(v0, v1, v2, barycentric, VERTEX_GET_TANGENT);
	v.bitangent = INTERPOLATE_BARYCENTRIC(v0, v1, v2, barycentric, VERTEX_GET_BITANGENT);
	v.uv = INTERPOLATE_BARYCENTRIC(v0, v1, v2, barycentric, VERTEX_GET_UV);

	LitMaterial material = Materials(instance.material_address).data[instance.material_index];
	payload.color = vec4(v.normal.xyz, 1.0);
}
