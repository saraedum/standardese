// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <boost/algorithm/string/replace.hpp>
#include <boost/type_index.hpp>

#include "../../external/catch/single_include/catch2/catch.hpp"

#include "../../standardese/transformation/link_target_internal_transformation.hpp"
#include "../../standardese/model/visitor/visit.hpp"
#include "../../standardese/model/markup/link.hpp"
#include "../../standardese/output_generator/xml/xml_generator.hpp"

#include "../util/logger.hpp"
#include "../util/cpp_file.hpp"
#include "../util/parsed_comments.hpp"

namespace standardese::test::transformation {

using standardese::test::util::cpp_file;

TEST_CASE("Links to Header Files are Resolved", "[link_target_internal_transformation]") {
  auto logger = util::logger::throwing_logger();
  cpp_file header;

  SECTION("A Link back to the Header File Itself") {
    auto parsed = util::parsed_comments(header).add(header, boost::algorithm::replace_all_copy(std::string(R"(
      \file
      [named header](<> "header.hpp")
      [relative path](<> "./header.hpp")
      [relative path](<> "../$path/header.hpp")
      [partial absolute path](<> "$path/header.hpp")
      )"), "$path", header.path().parent_path().filename().native()));

    standardese::transformation::link_target_internal_transformation{parsed.entities, header}.transform();

    CAPTURE(output_generator::xml::xml_generator::render(parsed));

    // Verify that all link could be resolved to C++ entities, namely the header file.
    for (auto& document: parsed.entities)
      model::visitor::visit([](auto&& link, auto&& recurse) {
        using T = std::decay_t<decltype(link)>;
        if constexpr (std::is_same_v<T, model::markup::link>) {
          link.target.accept([](auto&& target) -> void {
            using T = std::decay_t<decltype(target)>;
            CAPTURE(boost::typeindex::type_id<T>().pretty_name());
            REQUIRE(std::is_same_v<T, model::link_target::cppast_target>);
          });
        }

        recurse();
      }, document);
  }
}

}
