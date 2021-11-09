// Copyright (C) 2017 Jonathan Müller <jonathanmueller.dev@gmail.com>
//               2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <iostream>

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
        default:
          return;
      }
    }
    void flush_() override {}
  };

  return logger(std::shared_ptr<spdlog::sinks::sink>(new throwing_sink()), name);
}

}
