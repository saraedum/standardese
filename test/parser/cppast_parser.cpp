// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include "../external/catch/single_include/catch2/catch.hpp"

#include "../../standardese/parser/cppast_parser.hpp"

#include "../util/logger.hpp"
#include "../util/tmp_file.hpp"
#include "../util/unindent.hpp"

namespace standardese::test::parser {

using standardese::parser::cppast_parser;

TEST_CASE("Parsing a Simple Header File", "[cppast_parser]") {
  auto logger = util::logger::throwing_logger();

  auto header = util::tmp_file{"header.hpp", util::unindent(R"(
    /// Swap a and b.
    template <typename T>
    void swap(T& a, T& b);
  )")};

  cppast_parser parser{};
  const auto& parsed = parser.parse(header.path);

  CHECK(parsed.unmatched_comments().begin() == parsed.unmatched_comments().end());
}

TEST_CASE("Parse with compile_commands.json", "[cppast_parser]") {
  auto logger = util::logger::throwing_logger();

  auto header = util::tmp_file{"header.hpp", util::unindent(R"(
    #ifdef INCLUDE_COMMENTS
    /// A free comment.

    /// Another free comment.
    #endif
  )")};

  auto compile_commands = util::tmp_file{"compile_commands.json", util::unindent(R"(
    [
      {
        "directory": ")" + header.parent.path.native() + R"(",
        "arguments": ["c++", "-DINCLUDE_COMMENTS", "-c", "-o", "header.gch", "header.hpp"],
        "file": ")" + header.path.native() + R"("
      }
    ]
    )")};

  cppast_parser::cppast_parser_options options;
  options.compile_commands = compile_commands.path;

  cppast_parser parser{options};
  const auto& parsed = parser.parse(header.path);

  CHECK(parsed.unmatched_comments().begin() != parsed.unmatched_comments().end());
}

TEST_CASE("Parse with compile_flags.txt", "[cppast_parser]") {
  auto logstream = std::stringstream();
  auto logger = util::logger::capturing_logger(logstream);

  auto header = util::tmp_file{"header.hpp", util::unindent(R"(
    #ifdef INCLUDE_COMMENTS
    /// A free comment.

    /// Another free comment.
    #endif
  )")};

  auto compile_flags = util::tmp_file{"compile_flags.txt", util::unindent(R"(
    -DINCLUDE_COMMENTS
    )")};

  cppast_parser::cppast_parser_options options;
  options.compile_flags = compile_flags.path;

  cppast_parser parser{options};
  const auto& parsed = parser.parse(header.path);

  CHECK(logstream.str() != "");
  CHECK(logstream.str().find("compile_flags.txt") != std::string::npos);

  // compile_flags.txt is not supported by cppast yet. Eventually this should be "!=".
  CHECK(parsed.unmatched_comments().begin() == parsed.unmatched_comments().end());
}

}
