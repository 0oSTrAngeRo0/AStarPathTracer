#ifndef PATHTRACER_SOURCE_MATH_UTIL_H_
#define PATHTRACER_SOURCE_MATH_UTIL_H_

#include "math-utils.h"

namespace math {
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
