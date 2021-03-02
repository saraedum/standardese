// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//               2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_TOOL_OPTIONS_HPP_INCLUDED
#define STANDARDESE_TOOL_OPTIONS_HPP_INCLUDED

#include <string>

#include "transformations.hpp"
#include "parsers.hpp"
#include "document_builders.hpp"
#include "output_generators.hpp"

namespace standardese::tool {

/// The configuration that can be used when invoking the standardese
/// executable.
/// This combines all the configuration structs that the different
/// bits of the standardese API expect. When working with the API, there is
/// probably no reason to use this unless you want to parse command line
/// options or configuration files in the way that the standardese executable
/// does.
class options {
 public:
  /// Create options with the default values that the standardese executable uses.
  options();

  /// Options that control how option parsing itself works.
  struct options_options {
      /// Whether to include basic options that are specific to the standardese
      /// CLI such as `--help`, `--version`.
      bool include_cli_options = true;

      /// Whether to throw an `std::invalid_argument` exception when an illegal
      /// option was found; otherwise, we talk about the `--help` flag and `exit()`.)
      bool throw_on_error = false;

      /// The terminal width for help messages or `-1` to attempt to autodetect it.
      int columns = -1;

      /// The name of the executable, often the same as argv[0].
      std::string executable = "standardese";
  } options_options;

  /// Parse options from the command line.
  static options parse(int argc, const char* argv[], struct options_options);

  /// Options that control how source code and comments are parsed.
  struct tool::parsers::options parser_options;

  /// Options that control which output documents are created.
  struct tool::document_builders::options document_builder_options;

  /// Options that control transformations, i.e., how parsed markdown gets
  /// turned into the exact content of the output documents.
  struct tool::transformations::options transformation_options;

  /// Options that control how the output documents are emitted.
  struct tool::output_generators::options output_generator_options;
};

}

#endif
