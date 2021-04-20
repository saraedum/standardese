// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_MODEL_VISITOR_DETAIL_VISIT_HPP_INCLUDED
#define STANDARDESE_MODEL_VISITOR_DETAIL_VISIT_HPP_INCLUDED

#include <type_safe/optional.hpp>
#include <type_traits>
#include <functional>

#include <boost/type_index.hpp>

#include "../recursive_visitor.hpp"
#include "../generic_visitor.hpp"
#include "../recursion.hpp"

namespace standardese::model::visitor::detail {

template <typename T>
struct storage {
  T value;
};

template <>
struct storage<void> {};

template <typename T,
          bool is_const,
          bool is_recursive,
          typename R,
          typename B = std::conditional_t<is_recursive, model::visitor::recursive_visitor<is_const>, model::visitor::visitor<is_const>>>
struct visitor : T, storage<R>, model::visitor::generic_visitor<visitor<T, is_const, is_recursive, R>, B> {
  visitor(T t) : T(std::move(t)) {}

  template <typename E>
  void operator()(E&& e) {
    if constexpr (is_recursive) {
      if (T::operator()(std::forward<E>(e)) == recursion::RECURSE)
        model::visitor::recursive_visitor<is_const>::visit(e);
    } else if constexpr (std::is_same_v<R, void>) {
      T::operator()(std::forward<E>(e));
    } else {
      this->value = T::operator()(std::forward<E>(e));
    }
  }
};

template <typename T,
          typename E,
          bool is_const = std::is_const_v<std::remove_reference_t<E>>,
          typename R = typename std::invoke_result<T, model::markup::text>::type,
          bool is_recursive = std::is_same_v<std::decay_t<R>, recursion>,
          typename RR = std::conditional_t<is_recursive, void, R>>
auto visit(T&& lambda, E&& e) {
  visitor<T, is_const, is_recursive, RR> visitor{std::forward<T>(lambda)};
  e.accept(visitor);
  if constexpr (!std::is_same_v<RR, void>)
    return std::move(visitor.value);
}

}

#endif
