// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_THREADING_THREADED_POOL_HPP_INCLUDED
#define STANDARDESE_THREADING_THREADED_POOL_HPP_INCLUDED

#include <queue>
#include <mutex>

#include "pool.hpp"

namespace standardese::threading {


/// A multi-threaded worker pool with a fixed limited amount of parallelism.
class threaded_pool : public pool {
 public:
  /// Return a worker factory with the amount of parallelism.
  /// \param parallelism if non-positive, the value is selected automatically.
  threaded_pool(int parallelism);

  std::future<void> enqueue(std::function<void()> task) override;
  
  /// Return a worker factory with the amount of parallelism.
  /// \param parallelism if non-positive, the value is selected automatically.
  static factory factory(int parallelism);

  ~threaded_pool() override;

 private:
  int parallelism;
  int running = 0;

  bool accepting = true;

  std::queue<std::packaged_task<void()>> tasks;
  std::vector<std::thread> threads;

  std::condition_variable task_enqueued;
  std::condition_variable task_completed;

  std::mutex mutex;
};

}

#endif
