#ifndef PATHTRACER_SOURCE_ALGORITHM_H_
#define PATHTRACER_SOURCE_ALGORITHM_H_

#include <optional>

template <typename Container, typename T>
std::optional<T> Find(const Container& container, const T& target) {
	auto iter = std::find(container.begin(), container.end(), target);
	if (iter != container.end()) {
		return *iter;
	}
	return std::nullopt;
}

#endif // !PATHTRACER_SOURCE_ALGRITHM_H_
