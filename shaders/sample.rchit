#version 460
#extension GL_EXT_scalar_block_layout : enable
#extension GL_GOOGLE_include_directive : enable
#extension GL_EXT_shader_explicit_arithmetic_types_int32 : require
#extension GL_EXT_buffer_reference2 : require
#extension GL_EXT_ray_tracing : require
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require
#extension GL_EXT_shader_explicit_arithmetic_types_int16 : require

#include "Common.glsl"

layout(set = 0, binding = 2, buffer_reference, scalar) readonly buffer Vertices { Vertex data[]; };
layout(set = 0, binding = 3, buffer_reference, scalar) readonly buffer Indices { u32vec3 data[];};
layout(set = 0, binding = 4, buffer_reference, scalar) readonly buffer Materials { LitMaterial data[]; };
layout(set = 0, binding = 5, buffer_reference, scalar) readonly buffer Instances { InstanceData data[]; } instances;

layout(location = 0) rayPayloadInEXT HitPayload payload;
hitAttributeEXT vec3 attribs;

void main() {
  const vec3 barycentric = vec3(1.0 - attribs.x - attribs.y, attribs.x, attribs.y);
  InstanceData instance = instances.data[gl_InstanceCustomIndexEXT];
  LitMaterial material = Materials(instance.material_address).data[instance.material_index];
  payload.color = material.color;
}
