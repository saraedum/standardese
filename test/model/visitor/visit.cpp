// Copyright (C) 2017 Jonathan Müller <jonathanmueller.dev@gmail.com>
//               2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include "../../../external/catch/single_include/catch2/catch.hpp"

#include "../../../standardese/model/entity.hpp"
#include "../../../standardese/model/markup/paragraph.hpp"
#include "../../../standardese/model/markup/emphasis.hpp"
#include "../../../standardese/model/visitor/visit.hpp"

namespace stdandardese::test::model::visitor {

using standardese::model::visitor::visit;

TEST_CASE("Visitors Created from Lambdas", "[visitor]") {
  // A simple model corresponding to the MarkDown
  // ```
  // some text*some emphasized text*
  // ```
  standardese::model::entity root = standardese::model::markup::paragraph{
    standardese::model::markup::text("some text"),
    standardese::model::markup::emphasis(
      standardese::model::markup::text("some emphasized text")
    ),
  };

  const auto& croot = root;

  SECTION("Trivial Visitor") {
    visit([](auto&&) {}, root);
    visit([](auto&&) {}, croot);
  }

  SECTION("Visitor Returning Void") {
    int visits = 0;

    visit([&](auto&&) {
      visits++;
    }, root);
    visit([&](auto&&) {
      visits++;
    }, croot);

    REQUIRE(visits == 2);
  }

  SECTION("Visitor Returning Non-Void") {
    REQUIRE(visit([&](auto&&) {
      return 1;
    }, root) == 1);
    REQUIRE(visit([&](auto&&) {
      return 1;
    }, croot) == 1);
  }

  SECTION("Recursive Visitor") {
    int visits = 0;

    visit([&](auto&&) {
      visits++;
      return standardese::model::visitor::recursion::RECURSE;
    }, root);
    visit([&](auto&&) {
      visits++;
      return standardese::model::visitor::recursion::RECURSE;
    }, croot);

    REQUIRE(visits == 8);
  }

  SECTION("Non-Const Visitors") {
    visit([&](auto&& entity) {
      using T = std::decay_t<decltype(entity)>; 
      if constexpr (std::is_same_v<T, standardese::model::markup::text>) {
        REQUIRE(entity.value != "modified text");
      }
      return standardese::model::visitor::recursion::RECURSE;
    }, croot);

    SECTION("Non-Const Non-Recursive Void Visitor") {
      visit([&](auto&& entity) {
        using T = std::decay_t<decltype(entity)>; 
        if constexpr (std::is_same_v<T, standardese::model::markup::paragraph>) {
          entity.clear();
        }
      }, root);
    }

    SECTION("Non-Const Non-Recursive Non-Void Visitor") {
      REQUIRE(visit([&](auto&& entity) {
        using T = std::decay_t<decltype(entity)>; 
        if constexpr (std::is_same_v<T, standardese::model::markup::paragraph>) {
          entity.clear();
          return 1;
        }
        return 0;
      }, root) == 1);
    }

    SECTION("Non-Const Recursive Visitor") {
      visit([&](auto&& entity) {
        using T = std::decay_t<decltype(entity)>; 
        if constexpr (std::is_same_v<T, standardese::model::markup::text>) {
          entity.value = "modified text";
        }
        return standardese::model::visitor::recursion::RECURSE;
      }, root);
    }

    visit([&](auto&& entity) {
      using T = std::decay_t<decltype(entity)>; 
      if constexpr (std::is_same_v<T, standardese::model::markup::text>) {
        REQUIRE(entity.value == "modified text");
      }
      return standardese::model::visitor::recursion::RECURSE;
    }, croot);
  }
}

}
