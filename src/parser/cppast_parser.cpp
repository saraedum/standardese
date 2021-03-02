// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//               2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <cppast/diagnostic.hpp>
#include <cppast/forward.hpp>
#include  <type_safe/optional.hpp>

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

cppast_parser::options::options() {
  // Disable fast preprocessing as it skips header files completely because of
  // their header guards.
  clang_config.fast_preprocessing(false);
}

cppast_parser::cppast_parser(struct options options) : options(options), parser(cppast::libclang_parser(type_safe::ref(logger))) {
  if (options.compile_commands)
    this->compile_commands = cppast::libclang_compilation_database(options.compile_commands.value().generic_string());
}

const cppast::cpp_file& cppast_parser::parse(const boost::filesystem::path& source) {
  const auto config = compile_commands.map([&](const cppast::libclang_compilation_database& db) {
      return cppast::find_config_for(db, source.generic_string());
  }).value_or(options.clang_config);

  return context_.add(parser.parse(context_.index(), boost::filesystem::canonical(source).generic_string(), config));
}

const cpp_context& cppast_parser::context() const {
  return context_;
}

}
