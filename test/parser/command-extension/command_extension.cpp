// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <cmark-gfm.h>
#include <string>

#include "../../external/catch/single_include/catch2/catch.hpp"

#include "../../../src/parser/command-extension/command_extension.hpp"
#include "../../util/unindent.hpp"
#include "../../util/logger.hpp"
#include "../../../src/parser/cmark-extension/cmark_extension.hpp"

namespace standardese::test::parser::command_extension {

using standardese::parser::command_extension::command_extension;

class parser : public ::standardese::parser::markdown_parser {
 public:
  static auto parse(std::string comment) {
    comment = util::unindent(comment);

    using unique_parser = unique_cmark<cmark_parser, cmark_parser_free_with_extensions>;
    auto parser = unique_parser(cmark_parser_new(CMARK_OPT_SMART));

    command_extension::command_extension::create(parser.get());

    // Parse the comment into a tree of cmark nodes.
    cmark_parser_feed(parser.get(), comment.c_str(), comment.size());
    using unique_node = unique_cmark<cmark_node, cmark_node_free>;
    return unique_node(cmark_parser_finish(parser.get()));
  }
};

TEST_CASE("Inline Commands are Parsed") {
  auto logger = util::logger::throwing_logger();

  SECTION("Parameters are Parsed into Sibling Trees") {
    const auto root = parser::parse(R"(
      \param a This is the brief of parameter a.
      These are the details of parameter a.
      \param b This is the brief of parameter b.
      These are the details of parameter b.
      )");

    REQUIRE(standardese::parser::cmark_extension::cmark_extension::to_xml(root.get()) == util::unindent(R"(
      <?xml version="1.0" encoding="UTF-8"?>
      <!DOCTYPE document SYSTEM "CommonMark.dtd">
      <document xmlns="http://commonmark.org/xml/1.0">
        <inline>
          <section>
            <paragraph>
              <text xml:space="preserve">This is the brief of parameter a.</text>
            </paragraph>
          </section>
          <section>
            <paragraph>
              <text xml:space="preserve">These are the details of parameter a.</text>
            </paragraph>
          </section>
        </inline>
        <inline>
          <section>
            <paragraph>
              <text xml:space="preserve">This is the brief of parameter b.</text>
            </paragraph>
          </section>
          <section>
            <paragraph>
              <text xml:space="preserve">These are the details of parameter b.</text>
            </paragraph>
          </section>
        </inline>
      </document>
      )"));
  }

  SECTION("Modules are Stored in the Sibling Trees") {
    const auto root = parser::parse(R"(
      \module f
      \param a This is the brief of parameter a.
      These are the details of parameter a.
      \module a
      \param b This is the brief of parameter b.
      These are the details of parameter b.
      \module b
      )");

    REQUIRE(standardese::parser::cmark_extension::cmark_extension::to_xml(root.get()) == util::unindent(R"(
      <?xml version="1.0" encoding="UTF-8"?>
      <!DOCTYPE document SYSTEM "CommonMark.dtd">
      <document xmlns="http://commonmark.org/xml/1.0">
        <special-command />
        <inline>
          <section>
            <paragraph>
              <text xml:space="preserve">This is the brief of parameter a.</text>
            </paragraph>
          </section>
          <section>
            <paragraph>
              <text xml:space="preserve">These are the details of parameter a.</text>
            </paragraph>
          </section>
          <special-command />
        </inline>
        <inline>
          <section>
            <paragraph>
              <text xml:space="preserve">This is the brief of parameter b.</text>
            </paragraph>
          </section>
          <section>
            <paragraph>
              <text xml:space="preserve">These are the details of parameter b.</text>
            </paragraph>
          </section>
          <special-command />
        </inline>
      </document>
      )"));
  }
}

TEST_CASE("Section Commands are Parsed") {
  auto logger = util::logger::throwing_logger();

  SECTION("Explicit Sections Spanning Multiple Lines") {
    const auto root = parser::parse(R"(
      \brief Explicit brief.
      Still explicit brief.

      \details Explicit details.

      Still details.

      \effects Explicit effects.
      Still effects.

      Details again.

      \requires

      \returns
      Explicit returns.
      \returns Different returns.
      A backslash at the end of the line causes a hard line break and ends this section.\
      Details again.
      \notes Explicit notes.
      )");

    REQUIRE(standardese::parser::cmark_extension::cmark_extension::to_xml(root.get()) == util::unindent(R"(
      <?xml version="1.0" encoding="UTF-8"?>
      <!DOCTYPE document SYSTEM "CommonMark.dtd">
      <document xmlns="http://commonmark.org/xml/1.0">
        <section>
          <paragraph>
            <text xml:space="preserve">Explicit brief.</text>
            <softbreak />
            <text xml:space="preserve">Still explicit brief.</text>
          </paragraph>
        </section>
        <section>
          <paragraph>
            <text xml:space="preserve">Explicit details.</text>
          </paragraph>
          <paragraph>
            <text xml:space="preserve">Still details.</text>
          </paragraph>
          <paragraph>
            <text xml:space="preserve">Details again.</text>
          </paragraph>
          <paragraph>
            <text xml:space="preserve">Details again.</text>
          </paragraph>
        </section>
        <section>
          <paragraph>
            <text xml:space="preserve">Explicit effects.</text>
            <softbreak />
            <text xml:space="preserve">Still effects.</text>
          </paragraph>
        </section>
        <section />
        <section>
          <paragraph>
            <text xml:space="preserve">Explicit returns.</text>
          </paragraph>
        </section>
        <section>
          <paragraph>
            <text xml:space="preserve">Different returns.</text>
            <softbreak />
            <text xml:space="preserve">A backslash at the end of the line causes a hard line break and ends this section.</text>
          </paragraph>
        </section>
        <section>
          <paragraph>
            <text xml:space="preserve">Explicit notes.</text>
          </paragraph>
        </section>
      </document>
      )"));
  }
}

}
