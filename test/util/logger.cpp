// Copyright (C) 2017 Jonathan Müller <jonathanmueller.dev@gmail.com>
//               2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <iostream>
#include <cstdlib>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/base_sink.h>

#include "logger.hpp"

namespace standardese::test::util {

logger::logger(std::shared_ptr<spdlog::sinks::sink> sink, const std::string& name) {
  spdlog::drop(name);

  auto logger = std::shared_ptr<spdlog::logger>(new spdlog::logger(name, {sink}));
  spdlog::register_logger(logger);
  assert(logger == spdlog::get(name));

  logger->set_error_handler([](const auto& e) {
    throw message_logged_error(e);
  });

  const char* level = std::getenv("STANDARDESE_TEST_LOG_LEVEL");
  if (level != nullptr) {
    if (std::string(level) == "DEBUG")
      logger->set_level(spdlog::level::debug);
    else if (std::string(level) == "TRACE")
      logger->set_level(spdlog::level::trace);
    else
      logger->error(fmt::format("STANDARDESE_TEST_LOG_LEVEL environment variable must be unset or one of DEBUG or TRACE but found {}", std::string{level}));
  } else {
    logger->set_level(spdlog::level::warn);
  }
}

logger::~logger() {
  spdlog::drop(name);
}

logger logger::capturing_logger(std::ostream& stream, const std::string& name) {
  struct stream_sink : public spdlog::sinks::base_sink<std::mutex> {
    stream_sink(std::ostream& stream) : stream(stream) {}

    void sink_it_(const spdlog::details::log_msg& msg) override {
      stream << std::string(msg.payload.data(), msg.payload.size()) << std::endl;
    }

    void flush_() override {}

    std::ostream& stream;
  };

  return logger(std::shared_ptr<spdlog::sinks::sink>(new stream_sink(stream)), name);
}

logger logger::throwing_logger(const std::string& name) {
  struct throwing_sink : public spdlog::sinks::base_sink<std::mutex> {
    void sink_it_(const spdlog::details::log_msg& msg) override {
      switch (msg.level) {
        case spdlog::level::level_enum::critical:
        case spdlog::level::level_enum::err:
        case spdlog::level::level_enum::warn:
          throw message_logged_error(msg);
        case spdlog::level::level_enum::info:
          std::cerr << "info: " << std::string(msg.payload.data(), msg.payload.size()) << std::endl;
          break;
        case spdlog::level::level_enum::debug:
          std::cerr << "debug: " << std::string(msg.payload.data(), msg.payload.size()) << std::endl;
          break;
        default:
        case spdlog::level::level_enum::trace:
          std::cerr << "trace: " << std::string(msg.payload.data(), msg.payload.size()) << std::endl;
          break;
      }
    }

    void flush_() override {}
  };

  return logger(std::shared_ptr<spdlog::sinks::sink>(new throwing_sink()), name);
}

}
