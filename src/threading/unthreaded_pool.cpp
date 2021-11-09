// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include "../../standardese/threading/unthreaded_pool.hpp"
#include <exception>

namespace standardese::threading {

std::future<void> unthreaded_pool::enqueue(std::function<void()> task) {
  std::promise<void> promise;
  try {
    task();
    promise.set_value();
  } catch(...) {
    promise.set_exception(std::current_exception());
  }
  return promise.get_future();
}

pool::factory unthreaded_pool::factory = []() { return std::unique_ptr<pool>(new unthreaded_pool); };

}
