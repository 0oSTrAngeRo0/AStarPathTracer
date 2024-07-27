#pragma once

#include <unordered_map>
#include <optional>

template <typename TKey, typename TValue>
class StaticRegistry {
public:
	static void Register(const TKey& key, TValue value) {
		auto& map = GetMap();
		if (map.contains(key)) {
			throw std::runtime_error("Key Conflict");
		}
		map.insert_or_assign(key, value);
	}
	static std::optional<TValue> Get(const TKey& key) {
		auto& map = GetMap();
		if (!map.contains(key)) {
			throw std::runtime_error("Value not found");
		}
		return map.at(key);
	}
private:
	static std::unordered_map<TKey, TValue>& GetMap() {
		static std::unordered_map<TKey, TValue> map;
		return map;
	}
};