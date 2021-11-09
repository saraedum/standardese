// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include "../../standardese/threading/threaded_pool.hpp"

#include <stdexcept>
#include <cassert>

namespace standardese::threading {

threaded_pool::threaded_pool(int parallelism) {
  if (parallelism <= 0)
    parallelism = std::thread::hardware_concurrency() + 1;

  this->parallelism = parallelism;

  for (int i = 0; i < parallelism; i++) {
    threads.emplace_back([this]{
      while(true) {
        std::packaged_task<void()> task;

        {
          std::unique_lock lock{mutex};
          
          if (!accepting) {
            // Destructor has been called and there is nothing left to do.
            assert(tasks.empty());
            break;
          }

          if (!tasks.empty()) {
            task = std::move(tasks.front());
            tasks.pop();
            
            running++;
          } else {
            task_enqueued.wait(lock);
            continue;
          }
        }

        // Run the task. Note that this cannot throw. Any exceptions are stored
        // in the future that was created in enqueue.
        task();

        {
          std::unique_lock lock{mutex};

          running--;

          task_completed.notify_all();
        }
      }
    });
  }
}

pool::factory threaded_pool::factory(int parallelism) {
  return [parallelism]() { return std::unique_ptr<pool>(new threaded_pool(parallelism)); };
}

std::future<void> threaded_pool::enqueue(std::function<void()> task) {
  auto package = std::packaged_task<void()>(task);
  auto future = package.get_future();

  {
    std::lock_guard lock{mutex};

    if (!accepting)
      throw std::logic_error("Cannot enqueue tasks to stopped worker pool.");

    tasks.emplace(std::move(package));
  }

  task_enqueued.notify_one();

  return future;
}

threaded_pool::~threaded_pool() {
  // Wait for all threads to finish and the task queue to be empty.
  while(true) {
    std::unique_lock lock{mutex};

    if (running == 0 && tasks.empty()) {
      accepting = false;
      break;
    }

    task_completed.wait(lock);
  }

  // Wake up all threads, so that they see accepting=false and exit.
  task_enqueued.notify_all();

  // Clean up the threads.
  for (auto& thread : threads) thread.join();
}

}
