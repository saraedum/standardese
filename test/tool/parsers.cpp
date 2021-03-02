// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include "../../external/catch/single_include/catch2/catch.hpp"
#include "../../standardese/tool/parsers.hpp"
#include "../../standardese/model/unordered_entities.hpp"

namespace standardese::test::tool {

using standardese::tool::parsers;

TEST_CASE("Parsing Nothing Produces an Empty Set of Entities", "[tool]") {
  struct parsers parsers{{}};
  auto [parsed, context] = parsers.parse();

  CHECK(parsed.begin() == parsed.end());
}

/*
TEST_CASE("Parsing a Single Header File", "[tool]") {
  struct parsers parsers{{}};
  auto [parsed, context] = parsers.parse();

  CHECK(parsed.begin() != parsed.end());
}
*/

}
