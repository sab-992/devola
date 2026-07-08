#pragma once

#include <cstddef>
#include <gtest/gtest.h>
#include <tuple>
#include <utility>


template <typename T> struct TypesToTuple;
template <typename... Ts>
struct TypesToTuple<::testing::Types<Ts...>> {
  using type = std::tuple<Ts...>;
};

template <typename Tuple, typename F, std::size_t... I>
void ForEachTypeImpl(F&& f, std::index_sequence<I...>) {
  (f.template operator()<std::tuple_element_t<I, Tuple>>(), ...);
}

template <typename TypesList, typename F>
void ForEachType(F&& f) {
  using Tuple = typename TypesToTuple<TypesList>::type;
  ForEachTypeImpl<Tuple>(std::forward<F>(f),
      std::make_index_sequence<std::tuple_size_v<Tuple>>{});
}