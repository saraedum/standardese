// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include "../../../external/catch/single_include/catch2/catch.hpp"

#include "../../../standardese/output_generator/xml/xml_generator.hpp"
#include "../../../standardese/document_builder/entity_document_builder.hpp"
#include "../../../standardese/model/document.hpp"
#include "../../../standardese/parser/comment_parser.hpp"

#include "../../util/parsed_comments.hpp"
#include "../../util/unindent.hpp"
#include "../../util/logger.hpp"

namespace standardese::test::model::document
{

using util::unindent;
using standardese::output_generator::xml::xml_generator;

// TODO: This seems to be a test for the entity builder not for the entity document entity.

TEST_CASE("Entity Files can be Generated", "[document]")
{
  auto logger = util::logger::throwing_logger();
  document_builder::entity_document_builder builder; 

  SECTION("Entity Document from a Header Lists all Members")
  {
    util::cpp_file header(R"(
      #include <type_traits>

      namespace std
      {
          template <class T>
          void swap(T &a, T &b) noexcept(is_nothrow_move_constructible<T>::value &&
                                          is_nothrow_move_assignable<T>::value);
      }
      )");

    auto parsed = util::parsed_comments(header).add(header["std::swap"], R"(
      \effects Exchanges values stored in two locations.
      \requires Type `T` shall be `MoveConstructible` and `MoveAssignable`.
      )");

    parser::comment_parser({}, header).add_missing_sections(parsed.entities);

    auto document = builder(parsed[header], parsed.entities);
    document.name = "header.hpp";

    CHECK(xml_generator::render(document) == unindent(R"*(
      <?xml version="1.0"?>
      <document name="header.hpp">
        <entity-documentation>
          <entity-documentation>
            <entity-documentation>
              <section name="Effects">
                <paragraph>Exchanges values stored in two locations.</paragraph>
              </section>
              <section name="Requires">
                <paragraph>Type <code>T</code> shall be <code>MoveConstructible</code> and <code>MoveAssignable</code>.</paragraph>
              </section>
              <section name="Parameters" />
            </entity-documentation>
          </entity-documentation>
        </entity-documentation>
      </document>
      )*"));
  }
}

}
