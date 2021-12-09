// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include "../external/catch/single_include/catch2/catch.hpp"
#include "../../standardese/parser/comment_collector.hpp"
#include "../util/logger.hpp"
#include "../util/cpp_file.hpp"
#include "../util/unindent.hpp"

namespace standardese::test::parser {

using standardese::parser::comment_collector;

TEST_CASE("Collecting Comments", "[comment_collector]") {
  auto logger = util::logger::throwing_logger();

  SECTION("In an Empty Header File") {
    util::cpp_file header{""};

    comment_collector collector{};
    auto comments = collector.collect(header);

    REQUIRE(comments.size() == 1);
    
    comments.erase(std::remove_if(begin(comments), end(comments), [](const auto& comment) { return !comment.text; }), end(comments));

    REQUIRE(comments.size() == 0);
  }

  SECTION("In an Simple Header File") {
    util::cpp_file header{util::unindent(R"(
      /// Swap a and b.
      template <typename T>
      void swap(T& a, T& b);
      )")};

    comment_collector collector{};
    auto comments = collector.collect(header);

    REQUIRE(comments.size() == 2);

    comments.erase(std::remove_if(begin(comments), end(comments), [](const auto& comment) { return !comment.text; }), end(comments));

    REQUIRE(comments.size() == 1);
  }

  SECTION("In a Header with Free Comments") {
    util::cpp_file header{util::unindent(R"(
      /// A free comment.

      /// Swap a and b.
      template <typename T>
      void swap(T& a, T& b);

      /// Another free comment.

      // A comment that should be ignored.
      )")};

    comment_collector collector{};
    auto comments = collector.collect(header);

    REQUIRE(comments.size() == 4);

    comments.erase(std::remove_if(begin(comments), end(comments), [](const auto& comment) { return !comment.text; }), end(comments));

    REQUIRE(comments.size() == 3);
  }
}

}
