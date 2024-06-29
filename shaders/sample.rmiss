#version 460
#extension GL_EXT_ray_tracing : enable
#extension GL_GOOGLE_include_directive : enable

#include "Common.glsl"

layout(location = 0) rayPayloadInEXT HitPayload payload;

void main() {
  payload.color = vec4(vec3(0.3), 1.0);
}
