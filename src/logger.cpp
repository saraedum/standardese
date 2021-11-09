// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include <mutex>

#include "../standardese/logger.hpp"

namespace standardese::logger {

namespace {

std::mutex counter_mutex;

static bool is_warn_as_error = false;

int error_count = 0;
int warning_count = 0;

}

spdlog::logger& get() {
  const std::string name = "standardese";

  auto logger = spdlog::get(name);
  if (logger != nullptr)
    return *logger;

  logger = spdlog::stderr_color_mt(name);
  logger->set_pattern("%^[%l]%$ %v");
  if (spdlog::get(name) == nullptr) {
    // Automatic registration has been disabled, disable manually.
    spdlog::register_logger(logger);
  }

  return *logger;
}

void critical(const std::string& msg) { 
  get().critical(msg);

  std::lock_guard lock{counter_mutex};
  error_count++;
}

void error(const std::string& msg) { 
  get().error(msg);

  std::lock_guard lock{counter_mutex};
  error_count++;
}

void warn(const std::string& msg) { 
  if (is_warn_as_error)
    error(msg);
  else {
    get().warn(msg);

    std::lock_guard lock{counter_mutex};
    warning_count++;
  }
}

void info(const std::string& msg) { 
  get().info(msg);
}

void debug(const std::string& msg) { 
  get().debug(msg);
}

void trace(const std::string& msg) { 
  get().trace(msg);
}

void warn_as_error() {
  is_warn_as_error = true;
}

int errors() {
  std::lock_guard lock{counter_mutex};
  return error_count;
}

int warnings() {
  std::lock_guard lock{counter_mutex};
  return warning_count;
}

}
