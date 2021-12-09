// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//               2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_TOOL_PARSERS_HPP_INCLUDED
#define STANDARDESE_TOOL_PARSERS_HPP_INCLUDED

#include "../parser/cppast_parser.hpp"
#include "../parser/comment_collector.hpp"
#include "../parser/comment_parser.hpp"

namespace standardese::tool {

/// Parses source code and comments like the standardese executable does.
/// This wraps the source code and comment parsers that standardese uses into a single interface.
class parsers {
 public:
  struct parser_options {
    struct parser::cppast_parser::cppast_parser_options cppast_options;

    struct parser::comment_collector::comment_collector_options comment_collector_options;

    struct parser::comment_parser::comment_parser_options comment_parser_options;

    /// Header and MarkDown files to parse.
    std::vector<boost::filesystem::path> sources;

    /// The number of worker threads to run in parallel.
    /// When non-positive, the value is determined automatically.
    int parallelism = 0;
  };

  parsers(parser_options);

  /// Parse the source code and the comments and return a set of all the commented entities.
  std::pair<model::entity_set, parser::cpp_context> parse();

 private:
  struct parser_options options;
};

}

#endif
