// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <boost/type_index.hpp>

#include "../../external/catch/single_include/catch2/catch.hpp"

#include "../../standardese/transformation/link_external_legacy_transformation.hpp"
#include "../../standardese/model/visitor/visit.hpp"
#include "../../standardese/model/markup/link.hpp"

#include "../util/logger.hpp"
#include "../util/cpp_file.hpp"
#include "../util/parsed_comments.hpp"

namespace standardese::test::transformation {

using standardese::test::util::cpp_file;
using standardese::transformation::link_external_legacy_transformation;

TEST_CASE("External Legacy Legacy Links are Resolved", "[link_external_legacy_transformation]") {
  auto logger = util::logger::throwing_logger();
  cpp_file header;

  SECTION("A link to cppreference.com") {
    auto parsed = util::parsed_comments(header).add(header, R"(
      \file
      [a vector](<> "std::vector")
      [fully qualified vector](<> "::std::vector")
      )");

    struct link_external_legacy_transformation::options options;
    options.namspace = "std";
    options.url = R"(http://en.cppreference.com/mwiki/index.php?title=Special%3ASearch&search=$$)";

    standardese::transformation::link_external_legacy_transformation{parsed.entities, options}.transform();

    // Verify that all links could be resolved.
    for (auto& document: parsed.entities)
      model::visitor::visit([](auto&& link, auto&& recurse) {
        using T = std::decay_t<decltype(link)>;
        if constexpr (std::is_same_v<T, model::markup::link>) {
          link.target.accept([](auto&& target) -> void {
            using T = std::decay_t<decltype(target)>;
            CAPTURE(boost::typeindex::type_id<T>().pretty_name());
            if constexpr (std::is_same_v<T, model::link_target::uri_target>) {
              REQUIRE(target.uri == R"(http://en.cppreference.com/mwiki/index.php?title=Special%3ASearch&search=std::vector)");
            } else {
              REQUIRE(false);
            }
          });
        }

        recurse();
      }, document);
  }
}

}
