#pragma once

#include <entt/entt.hpp>
#include <optional>

namespace reflection {
	template <typename TReturn, typename... Args>
	inline auto CallFunc(entt::id_type type_id, entt::id_type func_id, Args... args) {
		using TReturnDecay = std::decay_t<TReturn>;
		constexpr bool is_void_return = std::is_void_v<TReturnDecay>;
		using TFuncReturn = std::conditional_t<is_void_return, void, std::optional<TReturnDecay>>;

		#define ASTAR_VOID_RETURN() if constexpr (is_void_return) { return; }
		#define ASTAR_OPT_RETURN(value) return TFuncReturn(value)
		#define ASTAR_RETURN_ERROR() ASTAR_VOID_RETURN() else { ASTAR_OPT_RETURN(std::nullopt); }

		auto type = entt::resolve(type_id);
		if (!type) ASTAR_RETURN_ERROR()
		auto func = type.func(func_id);
		if (!func) ASTAR_RETURN_ERROR()
		auto any = func.invoke({}, entt::forward_as_meta(args)...);
		ASTAR_VOID_RETURN()
		else {
			if constexpr (std::is_same_v<TReturnDecay, entt::meta_any>) ASTAR_OPT_RETURN(any);
			constexpr bool is_movable = std::is_move_constructible_v<TReturnDecay>;
			using TCasted = std::conditional_t<is_movable, TReturnDecay&&, TReturnDecay>;
			if (!any.allow_cast<TCasted>()) ASTAR_OPT_RETURN(std::nullopt);
			TCasted casted = any.cast<TCasted>();
			if constexpr (is_movable) ASTAR_OPT_RETURN(std::move(casted));
			else ASTAR_OPT_RETURN(casted);
		}

		#undef ASTAR_VOID_RETURN
		#undef ASTAR_RETURN_ERROR
		#undef ASTAR_OPT_RETURN
	}

	template <typename T>
	inline void RegisterTypeName(const std::string name) {
		entt::meta<T>().prop(entt::hashed_string("TypeName"), name);
	}

	inline std::optional<std::string> GetTypeName(entt::id_type id) {
		auto&& prop = entt::resolve(id).prop(entt::hashed_string("TypeName"));
		if (prop) return prop.value().cast<std::string>();
		else return std::nullopt;
	}
}