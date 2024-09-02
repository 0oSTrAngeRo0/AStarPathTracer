#ifndef SHADER_MATH_GLSL
#define SHADER_MATH_GLSL

#define PI 3.14159265359

#define INTERPOLATE_BARYCENTRIC(a, b, c, bary, func) (func(a) * bary.x + func(b) * bary.y + func(c) * bary.z)

// Generate a random unsigned int from two unsigned int values, using 16 pairs
// of rounds of the Tiny Encryption Algorithm. See Zafar, Olano, and Curtis,
// "GPU Random Numbers via the Tiny Encryption Algorithm"
uint Tea(uint val0, uint val1) {
    uint v0 = val0;
    uint v1 = val1;
    uint s0 = 0;

    for (uint n = 0; n < 16; n++) {
        s0 += 0x9e3779b9;
        v0 += ((v1 << 4) + 0xa341316c) ^ (v1 + s0) ^ ((v1 >> 5) + 0xc8013ea4);
        v1 += ((v0 << 4) + 0xad90777d) ^ (v0 + s0) ^ ((v0 >> 5) + 0x7e95761e);
    }

    return v0;
}

// Generate a random unsigned int in [0, 2^24) given the previous RNG state
// using the Numerical Recipes linear congruential generator
uint Lcg(inout uint prev) {
    const uint LCG_A = 1664525u;
    const uint LCG_C = 1013904223u;
    prev = (LCG_A * prev + LCG_C);
    return prev & 0x00FFFFFF;
}

// Generate a random float in [0, 1) given the previous RNG state
float Rnd(inout uint prev) {
    return (float(Lcg(prev)) / float(0x01000000));
}

vec3 HemiSphereSampleUniform(inout uint random_state) {
    float u = Rnd(random_state);
    float v = Rnd(random_state);
    float phi = v * 2.0 * PI;
    float cosine_theta = 1.0 - u;
    float sine_theta = sqrt(1.0 - cosine_theta * cosine_theta);
    return vec3(cos(phi) * sine_theta, sin(phi) * sine_theta, cosine_theta);
}

vec3 HemiSphereSampleCosineWeighted(inout uint random_state) {
    float u = Rnd(random_state);
    float v = Rnd(random_state);
    float phi = v * 2.0 * PI;
    float sine_theta = sqrt(u);
    float cosine_theta = sqrt(1 - u);
    return vec3(cos(phi) * sine_theta, sin(phi) * sine_theta, cosine_theta);
}

#endif