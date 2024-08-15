#pragma once

#include <vector>
#include <memory>
#include "Engine/Guid.h"
#include "Utilities/MacroUtilities.h"
#include <span>

class MemoryBlock {
public:
	template <typename T> inline T Get(size_t index) const {
		auto& vector = Reinterpret<T>();
		ASTAR_ASSERT(vector.size() >= index);
		return vector[index];
	}
	//template <typename T> inline T GetByOffset(size_t offset) const {
	//	using TData = std::decay_t<T>;
	//	ASTAR_ASSERT(size >= sizeof(TData) + offset);
	//	return *(((TData*)((std::byte*)data + offset)));
	//}
	template <typename T> inline size_t EmplaceBack(const T value) {
		size_t start = Size();
		auto& vector = Reinterpret<T>();
		vector.emplace_back(value);
		return start;
	}
	inline size_t Size() const { return data.size(); }
	template <typename T> inline const std::vector<T>& GetData() const {
		return Reinterpret<T>();
	}
protected:
	std::vector<std::byte> data;
	template <typename T> inline const std::vector<T>& Reinterpret() const {
		using TData = std::decay_t<T>;
		if constexpr (std::is_same_v<TData, std::byte>) {
			return data;
		}
		else {
			ASTAR_ASSERT(Size() % sizeof(TData) == 0);
			return reinterpret_cast<const std::vector<TData>&>(data);
		}
	}
	template <typename T> inline std::vector<T>& Reinterpret() {
		using TData = std::decay_t<T>;
		if constexpr (std::is_same_v<TData, std::byte>) {
			return data;
		}
		else {
			ASTAR_ASSERT(Size() % sizeof(TData) == 0);
			return reinterpret_cast<std::vector<TData>&>(data);
		}
	}
};
