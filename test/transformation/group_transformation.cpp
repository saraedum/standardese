// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include "../external/catch/single_include/catch2/catch.hpp"

#include "../../standardese/model/document.hpp"
#include "../../standardese/model/section.hpp"
#include "../../standardese/model/cpp_entity_documentation.hpp"
#include "../../standardese/parser/commands/section_command.hpp"
#include "../../standardese/transformation/group_transformation.hpp"
#include "../../standardese/model/unordered_entities.hpp"
#include "../../standardese/output_generator/xml/xml_generator.hpp"

#include "../util/cpp_file.hpp"
#include "../util/unindent.hpp"

namespace standardese::test::transformation {

TEST_CASE("Sections of Groups are Merged", "[group_transformation]") {
  SECTION("Trailing Group Members are Merged into the Principal Entity") {
    util::cpp_file header(R"(
      void f();
      void f(int);
      void f(char);
    )");

    auto principal = model::cpp_entity_documentation(header["f()"], header, {model::section(parser::commands::section_command::brief, {model::markup::text{"A brief for f"}})});
    auto secondary = model::cpp_entity_documentation(header["f(int)"], header);
    auto tertiary = model::cpp_entity_documentation(header["f(char)"], header);

    principal.group = secondary.group = tertiary.group = "group";

    auto documents = model::unordered_entities{model::document::anonymous({principal, secondary, tertiary})};

    standardese::transformation::group_transformation{documents, {}}.transform();

    REQUIRE(output_generator::xml::xml_generator::render(*documents.begin()) == util::unindent(R"(
    <?xml version="1.0"?>
    <document>
      <group-documentation>
        <section name="Brief">A brief for f</section>
      </group-documentation>
    </document>
    )"));
  }

  SECTION("Entities without a Group are left Alone") {
    util::cpp_file header(R"(
      void f();
      void f(int);
      void f(char);
    )");

    auto principal = model::cpp_entity_documentation(header["f()"], header, {model::section(parser::commands::section_command::brief, {model::markup::text{"A brief for f"}})});
    auto secondary = model::cpp_entity_documentation(header["f(int)"], header);
    auto tertiary = model::cpp_entity_documentation(header["f(char)"], header);

    principal.group = tertiary.group = "group";

    auto documents = model::unordered_entities{model::document::anonymous({principal, secondary, tertiary})};

    standardese::transformation::group_transformation{documents, {}}.transform();

    REQUIRE(output_generator::xml::xml_generator::render(*documents.begin()) == util::unindent(R"(
    <?xml version="1.0"?>
    <document>
      <group-documentation>
        <section name="Brief">A brief for f</section>
      </group-documentation>
      <entity-documentation name="f" />
    </document>
    )"));
  }
}

}
