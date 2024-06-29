#ifndef PATHTRACER_SOURCE_MATH_UTIL_H_
#define PATHTRACER_SOURCE_MATH_UTIL_H_

#include "math-utils.h"
#include <cstdint>

namespace math {
	inline uint32_t align_up(uint32_t size, uint32_t alignment) {
		return (size + (alignment - 1)) & (~(alignment - 1));
	}
	inline float Max(float a, float b) {
		return a > b ? a : b;
	}
	inline float Min(float a, float b) {
		return a < b ? a : b;
	}
	inline float Clamp(float value, float min, float max) {
		return Max(Min(value, max), min);
	}
}

#endif // !PATHTRACER_SOURCE_MATH_UTIL_H_
