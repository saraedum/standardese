// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_THREADING_FOR_EACH_HPP_INCLUDED
#define STANDARDESE_THREADING_FOR_EACH_HPP_INCLUDED

#include <vector>
#include <fmt/format.h>

#include "pool.hpp"
#include "../logger.hpp"

namespace standardese::threading {

/// Apply `f` to all the elements of the iterable.
/// Any exceptions are logged but not rethrown.
template <typename I, typename F>
void for_each(pool::factory workers, I begin, I end, F f) {
  std::vector<std::function<void()>> tasks;
  
  for (;begin != end; ++begin) {
    tasks.emplace_back([begin, &f]() {
      f(*begin);
    });
  }

  std::vector<std::future<void>> futures;

  {
    auto pool = workers();

    for (auto& task : tasks) futures.emplace_back(pool->enqueue(task));
  }

  // Wait for all the workers to finish and report any exceptions that have
  // happened in the workers.
  for (auto it = futures.begin(); it != futures.end(); ++it) {
    try {
      it->get();
    } catch (std::exception& e) {
      logger::error(e.what());
    }
  }
}

}

#endif

