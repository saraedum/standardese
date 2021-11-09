// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <fstream>

#include <boost/filesystem/path.hpp>

#include "../../../external/catch/single_include/catch2/catch.hpp"

#include "../../util/logger.hpp"

#include "../../../standardese/inventory/sphinx/documentation_set.hpp"
#include "../../../standardese/inventory/symbols.hpp"

using standardese::inventory::sphinx::documentation_set;
using standardese::inventory::symbols;

namespace standardese::test::inventory::sphinx {

TEST_CASE("Loading an Intersphinx Inventory", "[documentation_set]") {
    documentation_set inventory;

    SECTION("The Python 3 Inventory can be Loaded") {
      auto location = boost::filesystem::path(__FILE__).parent_path() / "../../../examples/links/doc/python.inv";
      CAPTURE(location);

      std::ifstream(location) >> inventory;

      symbols symbols{inventory};
      auto abc = symbols.find("abc.ABC");

      REQUIRE(abc);
      REQUIRE("py" == abc.value().accept([](auto&& target) -> std::string {
        using T = std::decay_t<decltype(target)>;
        if constexpr (std::is_same_v<T, model::link_target::sphinx_target>) {
          return target.entry.domain;
        } else {
          return "?";
        }
      }));
      REQUIRE("class" == abc.value().accept([](auto&& target) -> std::string {
        using T = std::decay_t<decltype(target)>;
        if constexpr (std::is_same_v<T, model::link_target::sphinx_target>) {
          return target.entry.type;
        } else {
          return "?";
        }
      }));
    }
}

TEST_CASE("Type Lookup up in Intersphinx Documentation Sets", "[documentation_set]") {
    auto logger = util::logger::throwing_logger();

    documentation_set inventory;
    inventory.entries.emplace_back("X", "c++", "type", 0," /X", "class X");

    symbols symbols{inventory};

    SECTION("Types can be Found by their Fully Qualified Name") {
      REQUIRE(symbols.find("X"));
      REQUIRE(symbols.find("::X"));
    }
}

}
