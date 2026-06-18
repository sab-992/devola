#pragma once

#include <type_traits>

// Taken from standard library.
template<typename U>
using underlyingType_t = std::underlying_type<U>::type;

template<typename T>
[[nodiscard]]
constexpr std::underlying_type<T>::type to_underlying(T value) noexcept { return static_cast<underlyingType_t<T>>(value); }


template<typename T>
[[nodiscard]]
constexpr T from_underlying(underlyingType_t<T> value) noexcept { return static_cast<T>(value); }