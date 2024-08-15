#ifndef SHADER_MATH_GLSL
#define SHADER_MATH_GLSL

#define INTERPOLATE_BARYCENTRIC(a, b, c, bary, func) (func(a) * bary.x + func(b) * bary.y + func(c) * bary.z)

#endif