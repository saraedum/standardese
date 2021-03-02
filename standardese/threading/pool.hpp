// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_THREADING_POOL_HPP_INCLUDED
#define STANDARDESE_THREADING_POOL_HPP_INCLUDED

#include <future>
#include <memory>

#include "../forward.hpp"

namespace standardese::threading {

/// A worker pool interface.
/// Standardese can run much of its tasks in parallel. How this parallelization
/// happens, can be controlled by providing a worker pool. The default pool
/// that standardese uses is the [unthreaded_pool]() which does not use any
/// threading.
class pool {
 public:
  /// Run `task` in the worker pool.
  virtual std::future<void> enqueue(std::function<void()> task) = 0;

  /// Wait for all tasks to finish and stop the worker pool.
  virtual ~pool();

  using factory = std::function<std::unique_ptr<pool>()>;
};

}

#endif
