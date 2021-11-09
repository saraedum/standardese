// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_THREADING_TRANSFORM_HPP_INCLUDED
#define STANDARDESE_THREADING_TRANSFORM_HPP_INCLUDED

#include <vector>
#include <optional>
#include <cassert>

#include "for_each.hpp"

namespace standardese::threading {

/// Apply `f` to all the elements of the iterable and the return the results
/// as a vector in the same order.
/// Any exceptions are logged and the corresponding entries in the output
/// vector are left default-initialized.
template <typename I, typename F>
auto transform(pool::factory workers, I begin, I end, F f) {
  std::vector<decltype(f(*begin))> results;
  std::vector<std::function<void()>> tasks;

  for (int i = 0; begin != end; ++begin, i++) {
    results.emplace_back();
    tasks.emplace_back([begin, i, &f, &results]() {
      results[i] = f(*begin);
    });
  }

  for_each(workers, tasks.begin(), tasks.end(), [](const auto& task) { task(); });

  return results;
}

}

#endif
