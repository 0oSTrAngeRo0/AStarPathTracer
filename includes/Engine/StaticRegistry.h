#pragma once

#include <unordered_map>
#include <optional>
#include <stdexcept>

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
		auto it = map.find(key);
		if (it == map.end()) return std::nullopt;
		return (*it).second;
	}
private:
	static std::unordered_map<TKey, TValue>& GetMap() {
		static std::unordered_map<TKey, TValue> map;
		return map;
	}
};

#include <functional>

template <typename TKey, typename TFunction>
class StaticFunctionRegistry;

template <typename TKey, typename TResult, typename ... TArguments>
class StaticFunctionRegistry<TKey, TResult(TArguments...)> : public StaticRegistry<TKey, std::function<TResult(TArguments...)>> {
public:
	using Base = StaticRegistry<TKey, std::function<TResult(TArguments...)>>;

	template <typename... TCallArguments>
    static TResult Call(const TKey& key, TCallArguments&&... args) {
        auto opt = Base::Get(key);
        if (!opt.has_value()) {
            throw std::runtime_error("Function not found for given key");
        }
        return (*opt)(std::forward<TCallArguments>(args)...);
    }
};


#include <vector>
#include <variant>

template <typename TKey, typename TLeaf>
class StaticTreeRegistry {
public:
	struct Node {
	public:
		TKey key;
		std::variant<TLeaf, std::vector<Node>> data;
	};

	using Key = TKey;
	using Leaf = TLeaf;
	using Branch = std::vector<Node>;

	static void AddLeaf(const TLeaf& leaf, std::initializer_list<TKey> paths, TKey key) {
		Node& root = GetRoot();
		Node* current = &root;
		for (const TKey& path : paths) {
			std::visit([&](auto&& arg) {
				using T = std::decay_t<decltype(arg)>;
				if constexpr (std::is_same_v<T, TLeaf>) {
					throw std::runtime_error("Invalid Key");
				}
				else if constexpr (std::is_same_v<T, Branch>) {
					auto iter = std::find_if(arg.begin(), arg.end(), [path](const Node& value) {
						return value.key == path;
					});
					if (iter == arg.end()) {
						Node node;
						node.key = path;
						node.data = Branch();
						arg.emplace_back(node);
						current = &arg.back();
					}
					else current = &(*iter);
				}
			}, current->data);
		}
		Node node;
		node.key = key;
		node.data = leaf;
		std::get<Branch>(current->data).emplace_back(node);
	}

	static Node& GetRoot() {
		static Node root = { TKey(), Branch()};
		return root;
	}

	static Branch& GetRootChildren() {
		return std::get<Branch>(GetRoot().data);
	}
};