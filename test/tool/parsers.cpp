// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include "../external/catch/single_include/catch2/catch.hpp"
#include "../../standardese/tool/parsers.hpp"
#include "../../standardese/model/entity_set.hpp"
#include "../util/logger.hpp"
#include "../util/tmp_file.hpp"
#include "../util/unindent.hpp"

namespace standardese::test::tool {

using standardese::tool::parsers;

TEST_CASE("Parsing Nothing Produces an Empty Set of Entities", "[tool][parsers]") {
  auto logger = util::logger::throwing_logger();

  struct parsers parsers{{}};
  auto [parsed, context] = parsers.parse();

  CHECK(parsed.begin() == parsed.end());
}

TEST_CASE("Parsing a Single Header File", "[tool][parsers]") {
  auto logger = util::logger::throwing_logger();

  auto header = util::tmp_file{"header.hpp", util::unindent(R"(
    /// Swap a and b.
    template <typename T>
    void swap(T& a, T& b);
    )")};

  parsers::parser_options options;
  options.sources.push_back(header.path);

  struct parsers parsers{options};
  // TODO(0.6.0-rc): There is an error here if the header file is empty.
  auto [parsed, context] = parsers.parse();

  CHECK(parsed.begin() != parsed.end());
}

// TODO(0.6.0-beta): Test that \entity works to refer to \unique_name. See
// similar test cases in comment_parser.cpp.

}
