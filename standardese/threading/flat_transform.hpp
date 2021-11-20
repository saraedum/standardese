// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_THREADING_FLAT_TRANSFORM_HPP_INCLUDED
#define STANDARDESE_THREADING_FLAT_TRANSFORM_HPP_INCLUDED

#include <vector>
#include <optional>
#include <cassert>

#include "transform.hpp"

namespace standardese::threading {

/// Apply `f` to all the elements of the iterable and the return the
/// results as a flattened vector in the same order.
/// Any exceptions are logged but not thrown.
template <typename I, typename F>
auto flat_transform(pool::factory workers, I begin, I end, F f) {
  std::vector<typename decltype(f(*begin))::value_type> results;
  for (auto& transformed: transform(workers, begin, end, f))
    results.insert(results.end(), std::make_move_iterator(transformed.begin()), std::make_move_iterator(transformed.end()));

  return results;
}

}

#endif

