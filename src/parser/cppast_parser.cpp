// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//               2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <cppast/diagnostic.hpp>
#include <cppast/cppast_fwd.hpp>

#include "../../standardese/parser/cppast_parser.hpp"
#include "../../standardese/threading/transform.hpp"
#include "../../standardese/logger.hpp"

namespace standardese::parser {

namespace {
struct cppast_logger : public cppast::diagnostic_logger {
  public:
    bool do_log(const char* source, const cppast::diagnostic& diagnostic) const override {
      const auto location = diagnostic.location.to_string();

      std::string message = diagnostic.message;
      if (!location.empty())
        message = location + " " + message;

      switch(diagnostic.severity) {
        case cppast::severity::critical:
          logger::critical(message);
          break;
        case cppast::severity::error:
          logger::error(message);
          break;
        case cppast::severity::warning:
          logger::warn(message);
          break;
        case cppast::severity::info:
          logger::info(message);
          break;
        case cppast::severity::debug:
          logger::debug(message);
          break;
      }

      return true;
    }
};

static cppast_logger logger;

}

cppast_parser::cppast_parser_options::cppast_parser_options() {
  // Disable fast preprocessing as it skips header files completely because of
  // their header guards.
  clang_config.fast_preprocessing(false);
}

cppast_parser::cppast_parser(cppast_parser_options options) : options(options), parser(cppast::libclang_parser(type_safe::ref(logger))) {
  if (options.compile_commands) {
    if (options.compile_commands.value().filename() != "compile_commands.json") {
      logger::error(fmt::format("Path {} does not point to a compilation database. Expected the filename to be compile_commands.json but found {}.", options.compile_commands.value().native(), options.compile_commands.value().filename().native()));
    } else {
      this->compile_commands = cppast::libclang_compilation_database(options.compile_commands.value().parent_path().native());
    }
  }
  if (options.compile_flags) {
    if (options.compile_commands) {
      logger::error("Cannot compile with compile_commands.json and compile_flags.txt. Ignoring the latter.");
    } else if (options.compile_flags.value().filename() != "compile_flags.txt") {
      logger::error(fmt::format("Path {} does not point to a compilation database. Expected the filename to be compile_flags.txt but found {}.", options.compile_flags.value().native(), options.compile_flags.value().filename().native()));
    } else {
      logger::warn(fmt::format("Ignoring compile_flags.txt at {} since they are not supported by cppast yet.", options.compile_flags.value().native()));
    }
  }
}

const cppast::cpp_file& cppast_parser::parse(const boost::filesystem::path& source) {
  const auto config = [&]() {
    if (compile_commands.has_value()) {
      const auto config = cppast::find_config_for(*compile_commands, source.generic_string());
      if (config.has_value())
        return config.value();
    }
    return options.clang_config;
  }();

  return context_.add(parser.parse(context_.index(), boost::filesystem::canonical(source).generic_string(), config));
}

const cpp_context& cppast_parser::context() const {
  return context_;
}

}
