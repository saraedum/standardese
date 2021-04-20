// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <boost/type_index.hpp>

#include "../../external/catch/single_include/catch2/catch.hpp"

#include "../../standardese/transformation/link_target_external_transformation.hpp"
#include "../../standardese/inventory/sphinx/documentation_set.hpp"
#include "../../standardese/inventory/symbols.hpp"
#include "../../standardese/model/visitor/visit.hpp"
#include "../../standardese/model/markup/link.hpp"
#include "../../standardese/output_generator/xml/xml_generator.hpp"

#include "../util/logger.hpp"
#include "../util/cpp_file.hpp"
#include "../util/parsed_comments.hpp"

namespace standardese::test::transformation {

using standardese::test::util::cpp_file;

TEST_CASE("Links to Sphinx Documentation Are Resolved", "[link_target_external_transformation]") {
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

    standardese::transformation::link_target_external_transformation{parsed.entities, inventory::symbols{inventory}}.transform();

    CAPTURE(output_generator::xml::xml_generator::render(parsed));

    // Verify that all links could be resolved.
    for (auto& document: parsed.entities)
      model::visitor::visit([](auto&& link) {
        using T = std::decay_t<decltype(link)>;
        if constexpr (std::is_same_v<T, model::markup::link>) {
          link.target.accept([](auto&& target) -> void {
            using T = std::decay_t<decltype(target)>;
            CAPTURE(boost::typeindex::type_id<T>().pretty_name());
            REQUIRE(std::is_same_v<T, model::link_target::sphinx_target>);
          });
        }
        return model::visitor::recursion::RECURSE;
      }, document);
  }
}

}

