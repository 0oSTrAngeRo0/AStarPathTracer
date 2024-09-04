#ifndef SHADER_RAY_CLOSEST_HIT_COMMON_GLSL
#define SHADER_RAY_CLOSEST_HIT_COMMON_GLSL

#extension GL_EXT_scalar_block_layout : enable
#extension GL_EXT_shader_explicit_arithmetic_types_int32 : require
#extension GL_EXT_buffer_reference2 : require
#extension GL_EXT_ray_tracing : require
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require
#extension GL_EXT_shader_explicit_arithmetic_types_int16 : require
#extension GL_EXT_debug_printf : enable

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

struct InstanceData {
	uint64_t vertex_position_address;
	uint64_t vertex_other_address;
	uint64_t index_address;
	uint64_t material_address;
	uint16_t material_index;
};

struct FetchedVertex {
	vec3 position;
	vec3 normal;
	vec3 tangent;
	vec3 bitangent;
	vec2 uv;
};

#define RAY_CLOSEST_HIT_DESCRIPTORS() \
layout(set = 0, binding = 2, buffer_reference, scalar) readonly buffer VerticesPosition { vec3 data[]; }; \
layout(set = 0, binding = 3, buffer_reference, scalar) readonly buffer VerticesOther { VertexOther data[]; }; \
layout(set = 0, binding = 4, buffer_reference, scalar) readonly buffer Indices { uvec3 data[]; }; \
layout(set = 0, binding = 6, buffer_reference, scalar) readonly buffer Instances { InstanceData data[]; } instances; \

#define RAY_CLOSEST_HIT_MATERIAL_DESCRIPTOR(material_type) \
layout(set = 0, binding = 5, buffer_reference, scalar) readonly buffer Materials { material_type data[]; }; \

#define RAY_CLOSEST_HIT_RAY_TRACING_VARIABLES() \
layout(location = 0) rayPayloadInEXT HitPayload payload; \
hitAttributeEXT vec2 attribs; \

#define RAY_CLOSEST_HIT_RAY_TRACING_FETCH_VERTEX(out_vertex, instance, index, barycentric) \
FetchedVertex out_vertex; \
do { \
	uint64_t vertex_position_address = instance.vertex_position_address; \
	vec3 p0 = VerticesPosition(vertex_position_address).data[index.x]; \
	vec3 p1 = VerticesPosition(vertex_position_address).data[index.y]; \
	vec3 p2 = VerticesPosition(vertex_position_address).data[index.z]; \
\
	uint64_t vertex_other_address = instance.vertex_other_address; \
	VertexOther v0 = VerticesOther(vertex_other_address).data[index.x]; \
	VertexOther v1 = VerticesOther(vertex_other_address).data[index.y]; \
	VertexOther v2 = VerticesOther(vertex_other_address).data[index.z]; \
\
	vec3 position = INTERPOLATE_BARYCENTRIC(p0, p1, p2, barycentric, DO_NOTHING); \
	vec3 normal = INTERPOLATE_BARYCENTRIC(v0, v1, v2, barycentric, VERTEX_GET_NORMAL); \
	vec3 tangent = INTERPOLATE_BARYCENTRIC(v0, v1, v2, barycentric, VERTEX_GET_TANGENT); \
	vec3 bitangent = INTERPOLATE_BARYCENTRIC(v0, v1, v2, barycentric, VERTEX_GET_BITANGENT); \
	vec2 uv = INTERPOLATE_BARYCENTRIC(v0, v1, v2, barycentric, VERTEX_GET_UV); \
\
	mat3x4 o2w = gl_ObjectToWorld3x4EXT; \
	position = (o2w * position).xyz; \
	normal = (o2w * normal).xyz; \
	tangent = (o2w * tangent).xyz; \
	bitangent = (o2w * bitangent).xyz; \
\
	out_vertex.position = position; \
	out_vertex.normal = normalize(normal); \
	out_vertex.tangent = normalize(tangent); \
	out_vertex.bitangent = normalize(bitangent); \
	out_vertex.uv = uv; \
} while(false) \

#define RAY_CLOSEST_HIT_RAY_TRACING_FETCH_INSTANCE_DATA(out_instance) \
InstanceData out_instance = instances.data[gl_InstanceCustomIndexEXT]; \

#define RAY_CLOSEST_HIT_RAY_TRACING_FETCH_BARYCENTRIC(out_barycentric) \
vec3 out_barycentric = vec3(1.0 - attribs.x - attribs.y, attribs.x, attribs.y); \

#define RAY_CLOSEST_HIT_RAY_TRACING_FETCH_INDEX(out_index) \
uvec3 out_index = Indices(instance.index_address).data[gl_PrimitiveID]; \

#define RAY_CLOSEST_HIT_RAY_TRACING_FETCH_MATERIAL_DATA(material_type, out_material, instance) \
material_type out_material = Materials(instance.material_address).data[instance.material_index]; \

#define RAY_CLOSEST_HIT_RAY_TRACING_FETCH_DATA(out_instance, out_barycentric, out_index, out_vertex, material_type, out_material) \
RAY_CLOSEST_HIT_RAY_TRACING_FETCH_INSTANCE_DATA(out_instance); \
RAY_CLOSEST_HIT_RAY_TRACING_FETCH_BARYCENTRIC(out_barycentric); \
RAY_CLOSEST_HIT_RAY_TRACING_FETCH_INDEX(out_index); \
RAY_CLOSEST_HIT_RAY_TRACING_FETCH_VERTEX(out_vertex, out_instance, out_index, out_barycentric); \
RAY_CLOSEST_HIT_RAY_TRACING_FETCH_MATERIAL_DATA(material_type, out_material, out_instance); \

void PrintVertex(VertexOther v) {
	debugPrintfEXT("normal:[%v3f], tangent:[%v4f], uv:[%v2f]\n", v.normal, v.tangent, v.uv);
}

#endif