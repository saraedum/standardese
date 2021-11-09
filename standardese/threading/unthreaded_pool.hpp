// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_THREADING_UNTHREADED_POOL_HPP_INCLUDED
#define STANDARDESE_THREADING_UNTHREADED_POOL_HPP_INCLUDED

#include "pool.hpp"

namespace standardese::threading {

/// The default worker pool for standardese which runs tasks in the current
/// thread.
class unthreaded_pool : public pool {
 public:
  std::future<void> enqueue(std::function<void()> task) override;

  static factory factory;
};

}

#endif

