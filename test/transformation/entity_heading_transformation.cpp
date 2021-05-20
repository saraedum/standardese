// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include "../../standardese/transformation/entity_heading_transformation.hpp"

#include "../../external/catch/single_include/catch2/catch.hpp"
#include "../../standardese/document_builder/entity_document_builder.hpp"
#include "../../standardese/output_generator/xml/xml_generator.hpp"
#include "../../standardese/model/document.hpp"
#include "../../standardese/model/visitor/visit.hpp"
#include "../util/logger.hpp"
#include "../util/cpp_file.hpp"
#include "../util/parsed_comments.hpp"
#include "../util/unindent.hpp"

namespace standardese::test::transformation {

using standardese::transformation::entity_heading_transformation;

TEST_CASE("Headings are Generated for C++ Entities", "[entity_heading_transformation]") {
  auto logger = util::logger::throwing_logger();

  const auto visit_heading = [](auto&& visitor, const auto& entities) {
    for (const auto& document : entities) {
      model::visitor::visit([&](auto&& heading, auto&& recurse) {
        using T = std::decay_t<decltype(heading)>;
        if constexpr (std::is_same_v<T, model::markup::heading>) {
          visitor(heading);
        }
        recurse();
      }, document);
    }
  };

  SECTION("Heading for a Header File") {
    util::cpp_file header;

    auto parsed = util::parsed_comments(header).add(header, R"(
      \file
      The brief description of the header file.
      The details of the header file.
      )");

    /* TODO
    SECTION("When the Format String is Empty, No Heading is Generated") {
      entity_heading_transformation{parsed.entities, {""}}.transform();

      CAPTURE(output_generator::xml::xml_generator::render(parsed));
      visit_heading([](auto&&) {
        // No heading should have been generated.
        REQUIRE(false);
      }, parsed.entities);
    }

    SECTION("The Format String must Describe a Heading") {
      REQUIRE_THROWS(entity_heading_transformation{parsed.entities, {"not a heading"}}.transform());
    }

    SECTION("The Format String can Contain the Header Name") {
      entity_heading_transformation{parsed.entities, {"# {{ name }}"}}.transform();

      REQUIRE(output_generator::xml::xml_generator::render(parsed) == util::unindent(R"(
        <?xml version="1.0"?>
        <entity-documentation name="header.hpp">
          <heading level="1">header.hpp</heading>
          <section name="Brief">
            <paragraph>The brief description of the header file.</paragraph>
          </section>
          <section name="Details">
            <paragraph>The details of the header file.</paragraph>
          </section>
        </entity-documentation>
        )"));
    }

    SECTION("If the Format String Contains the Brief, it is Omitted Later") {
      entity_heading_transformation{parsed.entities, {R"(# {{ name }} — {{ md(section("brief")) }}{{ drop_section("brief") }})"}}.transform();

      REQUIRE(output_generator::xml::xml_generator::render(parsed) == util::unindent(R"(
        <?xml version="1.0"?>
        <entity-documentation name="header.hpp">
          <heading level="1">header.hpp — The brief description of the header file.</heading>
          <section name="Details">
            <paragraph>The details of the header file.</paragraph>
          </section>
        </entity-documentation>
        )"));
    }
    */
  }

}

}
