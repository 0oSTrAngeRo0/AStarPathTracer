#version 460
#extension GL_EXT_ray_tracing : enable
#extension GL_GOOGLE_include_directive : enable

#include "../Includes/Common.glsl"

layout(location = 0) rayPayloadInEXT HitPayload payload;

void main() {
  payload.emittence = vec3(0);
  payload.throughput = vec3(1);
  payload.is_stopped = true;
}
