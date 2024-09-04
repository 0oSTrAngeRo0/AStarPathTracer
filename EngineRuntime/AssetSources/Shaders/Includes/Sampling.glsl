#ifndef SAMPLING_HLSL
#define SAMPLING_HLSL

#include "Constant.hlsl"
#include "Illumination.hlsl"
#include "Random.hlsl"
#include "Vertex.hlsl"

float3 HemiSphereSampleUniform(float u, float v) {
    float phi = v * 2.0 * PI;
    float cosTheta = 1.0 - u;
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);
    return float3(cos(phi) * sinTheta, sin(phi) * sinTheta, cosTheta);
}

float3 HemiSphereSampleCosineWeighted(float u, float v) {
    float phi = v * (2.0 * PI);
    float sine = sqrt(u);
    return float3(cos(phi) * sine, sin(phi) * sine, sqrt(1 - u));
}

void SampleLambertUniform(in Vertex vertex, inout MainRayPayload payload, out float3 direction, in float3 albedo,
    out float3 color) {
    float cosine = dot(vertex.Normal, normalize(-WorldRayDirection()));
    float2 sample = float2(RandomFloat01(payload.RngState), RandomFloat01(payload.RngState));
    direction = HemiSphereSampleUniform(sample.x, sample.y);

#if 1
    float3 b = normalize(cross(vertex.Normal, vertex.Tangent));
    direction = vertex.Tangent * direction.x + b * direction.y + vertex.Normal * direction.z;
    direction = normalize(direction);
#else
    direction = normalize(vertex.Normal + direction);
#endif

    color = albedo * cosine * 2;
}

void SampleLambertCosineWeighted(in Vertex vertex, inout MainRayPayload payload, out float3 direction, in float3 albedo,
    out float3 color) {
    float2 sample = float2(RandomFloat01(payload.RngState), RandomFloat01(payload.RngState));
    direction = HemiSphereSampleCosineWeighted(sample.x, sample.y);
    float3 b = normalize(cross(vertex.Normal, vertex.Tangent));
    direction = vertex.Tangent * direction.x + b * direction.y + vertex.Normal * direction.z;
    direction = normalize(direction);
    color = albedo;
}

void SampleMicrofacetUniform(in Vertex vertex, in MainRayPayload payload, out float3 direction, in float d, in float g, in float3 f,
    out float3 color) {
    float2 sample = float2(RandomFloat01(payload.RngState), RandomFloat01(payload.RngState));
    direction = HemiSphereSampleCosineWeighted(sample.x, sample.y);
    float3 b = normalize(cross(vertex.Normal, vertex.Tangent));
    direction = vertex.Tangent * direction.x + b * direction.y + vertex.Normal * direction.z;
    direction = normalize(direction);

    float3 view = normalize(-WorldRayDirection());
    float3 normal = vertex.Normal;
    float nov = dot(view, normal);

    float distribution = DistributionGGX(direction, view, normal, d);
    float geometry = GeometrySmithGgx(direction, view, normal, g);
    float3 fresnel = FresnelSchlick(direction, view, normal, f.xyz);
    float3 specular = distribution * geometry * fresnel / (4 * dot(normal, direction) * nov);
    color = PI * specular + f;
}

#endif
