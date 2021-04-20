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

template <typename T>
auto visit(T&& lambda, model::entity& e) {
  return detail::visit(std::forward<T>(lambda), e);
}

template <typename T>
auto visit(T&& lambda, const model::entity& e) {
  return detail::visit(std::forward<T>(lambda), e);
}

}

#endif
