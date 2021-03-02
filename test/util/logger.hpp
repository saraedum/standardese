// Copyright (C) 2017 Jonathan Müller <jonathanmueller.dev@gmail.com>
//               2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_TEST_UTIL_LOGGER_HPP_INCLUDED
#define STANDARDESE_TEST_UTIL_LOGGER_HPP_INCLUDED

#include <spdlog/sinks/sink.h>

namespace standardese::test::util {

/// Utilities to intercept log messages during testing.
/// Usually we want to throw an exception whenever an (unexpected) log messages has been logged.
struct logger {
  logger(std::shared_ptr<spdlog::sinks::sink> sink, const std::string& name="standardese");

  ~logger();

  struct message_logged_error : public std::runtime_error {
    message_logged_error(const spdlog::details::log_msg& msg) : std::runtime_error(msg.payload.data()) {}
    message_logged_error(const std::string& msg) : std::runtime_error(msg) {}
  };

  /// Register a logger that captures all messages in a stream and return the logger and the stream.
  static logger capturing_logger(std::ostream& stream, const std::string& name="standardese");

  /// Register an spdlog logger that throws an exception for any message logged.
  static logger throwing_logger(const std::string& name="standardese");

 private:
  std::string name;
};

}

#endif // STANDARDESE_TEST_LOGGER_HPP_INCLUDED
