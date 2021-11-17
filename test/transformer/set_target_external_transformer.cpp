// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <boost/type_index.hpp>

#include "../external/catch/single_include/catch2/catch.hpp"

#include "../../standardese/transformer/set_target_external_transformer.hpp"
#include "../../standardese/inventory/sphinx/documentation_set.hpp"
#include "../../standardese/inventory/symbols.hpp"
#include "../../standardese/model/visitor/visit.hpp"
#include "../../standardese/model/markup/link.hpp"
#include "../../standardese/output_generator/xml/xml_generator.hpp"
#include "../../standardese/transformer/warn_target_unresolved_transformer.hpp"

#include "../util/logger.hpp"
#include "../util/cpp_file.hpp"
#include "../util/parsed_comments.hpp"

namespace standardese::test::transformer {

using standardese::test::util::cpp_file;
using standardese::transformer::set_target_external_transformer;

TEST_CASE("Links to Sphinx Documentation Are Resolved", "[set_target_external_transformer]") {
  auto logger = util::logger::throwing_logger();
  cpp_file header;

  SECTION("A Link to a Sphinx Type") {
    auto parsed = util::parsed_comments(header).add(header, R"(
      \file
      [the C++ type X](<> "X")
      [the fully qualified C++ type X](<> "::X")
      )");

    standardese::inventory::sphinx::documentation_set inventory;
    inventory.entries.emplace_back("X", "c++", "type", 0," /X", "class X");

    SECTION("All Links can be Resolved") {
      set_target_external_transformer{&parsed.entities, inventory::symbols{&inventory}}.transform();

      CAPTURE(output_generator::xml::xml_generator::render(parsed));

      REQUIRE_NOTHROW(standardese::transformer::warn_target_unresolved_transformer{&parsed.entities}.transform());
    }

    SECTION("No Links can be Resolved when Schemata are Required") {
      set_target_external_transformer::set_target_external_transformer_options options;
      options.require_schema = true;

      set_target_external_transformer{&parsed.entities, inventory::symbols{&inventory}, options}.transform();

      CAPTURE(output_generator::xml::xml_generator::render(parsed));

      REQUIRE_THROWS(standardese::transformer::warn_target_unresolved_transformer{&parsed.entities}.transform());
    }
  }

  SECTION("An Explicit Link to a Sphinx Type") {
    auto parsed = util::parsed_comments(header).add(header, R"(
      \file
      [the C++ type X](<> "xxx://X")
      [the fully qualified C++ type X](<> "xxx://::X")
      )");

    standardese::inventory::sphinx::documentation_set inventory;
    inventory.entries.emplace_back("X", "c++", "type", 0," /X", "class X");

    SECTION("No Links can be Resolved without a Configured Schema") {
      set_target_external_transformer{&parsed.entities, inventory::symbols{&inventory}}.transform();

      CAPTURE(output_generator::xml::xml_generator::render(parsed));

      REQUIRE_THROWS(standardese::transformer::warn_target_unresolved_transformer{&parsed.entities}.transform());
    }

    SECTION("All Links can be Resolved with the Correct Scheme") {
      set_target_external_transformer::set_target_external_transformer_options options;
      options.schema = "xxx";

      set_target_external_transformer{&parsed.entities, inventory::symbols{&inventory}, options}.transform();

      CAPTURE(output_generator::xml::xml_generator::render(parsed));

      REQUIRE_NOTHROW(standardese::transformer::warn_target_unresolved_transformer{&parsed.entities}.transform());
    }
  }
}

}

