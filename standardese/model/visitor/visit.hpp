// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_MODEL_VISITOR_VISIT_HPP_INCLUDED
#define STANDARDESE_MODEL_VISITOR_VISIT_HPP_INCLUDED

#include "visitor.hpp"
#include "detail/visit.hpp"
#include "../entity.hpp"

namespace standardese::model::visitor
{

template <typename ...Callbacks>
auto visit(entity& entity, Callbacks&&... callbacks) {
  using T = detail::visit_return_t<Callbacks...>;
  using visitors = detail::partial_visitors<false, Callbacks...>;
  return detail::functional_visitor<false, T, typename visitors::template visitor_t<Callbacks>...>(std::forward<Callbacks>(callbacks)...)(entity);
}

template <typename ...Callbacks>
inline auto visit(const entity& entity, Callbacks&&... callbacks) {
  using T = detail::visit_return_t<Callbacks...>;
  using visitors = detail::partial_visitors<true, Callbacks...>;
  return detail::functional_visitor<true, T, typename visitors::template visitor_t<Callbacks>...>(std::forward<Callbacks>(callbacks)...)(entity);
}

}

#endif
