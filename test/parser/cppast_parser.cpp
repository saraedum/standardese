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

}
