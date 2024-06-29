#pragma once

#include <optional>
#include <vector>
#include <functional>

template <typename Container, typename T>
std::optional<T> Find(const Container& container, const T& target) {
	auto iter = std::find(container.begin(), container.end(), target);
	if (iter != container.end()) {
		return *iter;
	}
	return std::nullopt;
}

template <typename TIn, typename TOut>
std::vector<TOut> Select(const std::vector<TIn>& source, std::function<TOut(const TIn&)> function) {
	std::vector<TOut> result;
	for (const TIn& element : source) {
		TOut target = function(element);
		result.emplace_back(target);
	}
	return result;
}
