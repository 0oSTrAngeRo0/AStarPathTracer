#pragma once

#include <type_traits>

template <typename TEnum>
concept EnumClass = std::is_enum_v<TEnum> && !std::is_convertible_v<TEnum, int>;

template <typename TEnum, TEnum Value> 
requires EnumClass<TEnum>
constexpr auto GetEnumUnderlying() {
	return static_cast<std::underlying_type_t<TEnum>>(Value);
}

template <typename TEnum>
requires EnumClass<TEnum>
auto GetEnumUnderlying(TEnum value) {
	return static_cast<std::underlying_type_t<TEnum>>(value);
}