// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include "../external/catch/single_include/catch2/catch.hpp"

#include "../../standardese/output_generator/xml/xml_generator.hpp"
#include "../../standardese/transformer/create_entity_document_transformer.hpp"
#include "../../standardese/model/document.hpp"
#include "../../standardese/parser/comment_parser.hpp"

#include "../util/parsed_comments.hpp"
#include "../util/unindent.hpp"
#include "../util/logger.hpp"

namespace standardese::test::document_builder
{

using util::unindent;
using standardese::output_generator::xml::xml_generator;
using standardese::transformer::create_entity_document_transformer;

TEST_CASE("Entity Documents can be Generated", "[entity_document_builder]")
{
  auto logger = util::logger::throwing_logger();

  SECTION("Entity Documents for Templated Functions") {
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
      \tparam T Type `T` shall be `MoveConstructible` and `MoveAssignable`.
      )");

    SECTION("Entity Document for a Header Lists all Members")
    {
      auto options = create_entity_document_transformer::create_entity_document_transformer_options{};
      options.document_name = "doc_header";
      options.document_path = "doc_header";

      auto documents = create_entity_document_transformer{&parsed.entities, header, options}.transform();
      REQUIRE(documents.size() == 1);
      auto document = documents.begin()->as<model::document>();
      document.name = "header.hpp";

      CHECK(xml_generator::render(document) == unindent(R"*(
        <?xml version="1.0"?>
        <document name="header.hpp">
          <entity-documentation name="header.hpp">
            <entity-documentation name="std">
              <entity-documentation name="swap">
                <section name="Effects">
                  <paragraph>Exchanges values stored in two locations.</paragraph>
                </section>
                <section name="Requires">
                  <entity-documentation name="T">
                    <section name="Brief">
                      <paragraph>Type <code>T</code> shall be <code>MoveConstructible</code> and <code>MoveAssignable</code>.</paragraph>
                    </section>
                  </entity-documentation>
                </section>
                <section name="Parameters">
                  <entity-documentation name="a" />
                  <entity-documentation name="b" />
                </section>
              </entity-documentation>
            </entity-documentation>
          </entity-documentation>
        </document>
        )*"));
    }

    SECTION("Entity Document for a Non-Header")
    {
      auto options = create_entity_document_transformer::create_entity_document_transformer_options{};
      options.document_name = "doc_swap";
      options.document_path = "doc_swap";
      options.filter = [&](const cppast::cpp_entity& entity) { return &entity == &header["std::swap"]; };

      REQUIRE(options.filter(header["std::swap"]));

      const auto documents = create_entity_document_transformer{&parsed.entities, header, options}.transform();
      REQUIRE(documents.size() == 1);
      const auto document = documents.begin()->as<model::document>();

      CHECK(xml_generator::render(document) == unindent(R"*(
        <?xml version="1.0"?>
        <document name="doc_swap">
          <entity-documentation name="swap">
            <section name="Effects">
              <paragraph>Exchanges values stored in two locations.</paragraph>
            </section>
            <section name="Requires">
              <entity-documentation name="T">
                <section name="Brief">
                  <paragraph>Type <code>T</code> shall be <code>MoveConstructible</code> and <code>MoveAssignable</code>.</paragraph>
                </section>
              </entity-documentation>
            </section>
            <section name="Parameters">
              <entity-documentation name="a" />
              <entity-documentation name="b" />
            </section>
          </entity-documentation>
        </document>
        )*"));
    }
  }

  SECTION("Entity Documents For Types") {
    SECTION("Entity Document for a Class") {
      util::cpp_file header(R"(
        class X{};
        )");

      auto parsed = util::parsed_comments(header).add(header["X"], "A class.");

      auto options = create_entity_document_transformer::create_entity_document_transformer_options{};
      options.document_name = "doc_X";
      options.document_path = "doc_X";
      options.filter = [&](const cppast::cpp_entity& entity) { return &entity == &header["X"]; };

      const auto documents = create_entity_document_transformer{&parsed.entities, header, options}.transform();
      REQUIRE(documents.size() == 1);
      const auto document = documents.begin()->as<model::document>();

      CHECK(xml_generator::render(document) == unindent(R"*(
        <?xml version="1.0"?>
        <document name="doc_X">
          <entity-documentation name="X">
            <section name="Brief">
              <paragraph>A class.</paragraph>
            </section>
          </entity-documentation>
        </document>
        )*"));
    }

    SECTION("Entity Document for a Class Member") {
      util::cpp_file header(R"(
        class X{
          int a;

          public:
          int b;
        };
        )");

      SECTION("Private Class Members") {
        auto parsed = util::parsed_comments(header).add(header["X.a"], "The a of X.");

        auto options = create_entity_document_transformer::create_entity_document_transformer_options{};
        options.document_name = "doc_a";
        options.document_path = "doc_a";
        options.filter = [&](const cppast::cpp_entity& entity) { return &entity == &header["X.a"]; };

        const auto documents = create_entity_document_transformer{&parsed.entities, header, options}.transform();
        REQUIRE(documents.size() == 1);
        const auto document = documents.begin()->as<model::document>();

        CHECK(xml_generator::render(document) == unindent(R"*(
          <?xml version="1.0"?>
          <document name="doc_a">
            <entity-documentation name="a">
              <section name="Brief">
                <paragraph>The a of X.</paragraph>
              </section>
            </entity-documentation>
          </document>
          )*"));
      }

      SECTION("Public Class Members") {
        auto parsed = util::parsed_comments(header).add(header["X.b"], "The b of X.");

        auto options = create_entity_document_transformer::create_entity_document_transformer_options{};
        options.document_name = "doc_b";
        options.document_path = "doc_b";
        options.filter = [&](const cppast::cpp_entity& entity) { return &entity == &header["X.b"]; };

        const auto documents = create_entity_document_transformer{&parsed.entities, header, options}.transform();
        REQUIRE(documents.size() == 1);
        const auto document = documents.begin()->as<model::document>();

        CHECK(xml_generator::render(document) == unindent(R"*(
          <?xml version="1.0"?>
          <document name="doc_b">
            <entity-documentation name="b">
              <section name="Brief">
                <paragraph>The b of X.</paragraph>
              </section>
            </entity-documentation>
          </document>
          )*"));
      }
    }

    SECTION("Entity Document for a Class Template") {
      util::cpp_file header(R"(
        template <typename T>
        class X{};
        )");

      auto parsed = util::parsed_comments(header).add(header["X"], "A class template.");

      auto options = create_entity_document_transformer::create_entity_document_transformer_options{};
      options.document_name = "doc_X";
      options.document_path = "doc_X";
      options.filter = [&](const cppast::cpp_entity& entity) { return &entity == &header["X"]; };

      const auto documents = create_entity_document_transformer{&parsed.entities, header, options}.transform();
      REQUIRE(documents.size() == 1);
      const auto document = documents.begin()->as<model::document>();

      CHECK(xml_generator::render(document) == unindent(R"*(
        <?xml version="1.0"?>
        <document name="doc_X">
          <entity-documentation name="X">
            <section name="Brief">
              <paragraph>A class template.</paragraph>
            </section>
            <section name="Requires">
              <entity-documentation name="T" />
            </section>
          </entity-documentation>
        </document>
        )*"));
    }

    SECTION("Entity Document for a Class Template Inheriting Publicly") {
      util::cpp_file header(R"(
        class X{};

        template <typename T>
        class Y{};

        template <typename T>
        class Z: public X, public Y<T> {};
        )");

      auto parsed = util::parsed_comments(header).add(header["Z"], R"(
        A class template.
        \base X Inherits from X.
        \base Y<T> Inherits from Y.
        )");

      auto options = create_entity_document_transformer::create_entity_document_transformer_options{};
      options.document_name = "doc_Z";
      options.document_path = "doc_Z";
      options.filter = [&](const cppast::cpp_entity& entity) { return &entity == &header["Z"]; };

      const auto documents = create_entity_document_transformer{&parsed.entities, header, options}.transform();
      REQUIRE(documents.size() == 1);
      const auto document = documents.begin()->as<model::document>();

      CHECK(xml_generator::render(document) == unindent(R"*(
        <?xml version="1.0"?>
        <document name="doc_Z">
          <entity-documentation name="Z">
            <section name="Brief">
              <paragraph>A class template.</paragraph>
            </section>
            <section name="Requires">
              <entity-documentation name="T" />
            </section>
            <section name="Base classes">
              <entity-documentation name="X">
                <section name="Brief">
                  <paragraph>Inherits from X.</paragraph>
                </section>
              </entity-documentation>
              <entity-documentation name="Y&lt;T>">
                <section name="Brief">
                  <paragraph>Inherits from Y.</paragraph>
                </section>
              </entity-documentation>
            </section>
          </entity-documentation>
        </document>
        )*"));
    }

    SECTION("Entity Document for a Class Template Inheriting Non-Publicly") {
      util::cpp_file header(R"(
        class X{};

        template <typename T>
        class Y{};

        template <typename T>
        class Z: protected X, private Y<T> {};
        )");

      auto parsed = util::parsed_comments(header).add(header["Z"], R"(
          A class template.
          \base X Inherits from X.
          \base Y<T> Inherits from Y.
          )");

      auto options = create_entity_document_transformer::create_entity_document_transformer_options{};
      options.document_name = "doc_Z";
      options.document_path = "doc_Z";
      options.filter = [&](const cppast::cpp_entity& entity) { return &entity == &header["Z"]; };

      const auto documents = create_entity_document_transformer{&parsed.entities, header, options}.transform();
      REQUIRE(documents.size() == 1);
      const auto document = documents.begin()->as<model::document>();

      CHECK(xml_generator::render(document) == unindent(R"*(
          <?xml version="1.0"?>
          <document name="doc_Z">
            <entity-documentation name="Z">
              <section name="Brief">
                <paragraph>A class template.</paragraph>
              </section>
              <section name="Requires">
                <entity-documentation name="T" />
              </section>
              <section name="Base classes">
                <entity-documentation name="X">
                  <section name="Brief">
                    <paragraph>Inherits from X.</paragraph>
                  </section>
                </entity-documentation>
                <entity-documentation name="Y&lt;T>">
                  <section name="Brief">
                    <paragraph>Inherits from Y.</paragraph>
                  </section>
                </entity-documentation>
              </section>
            </entity-documentation>
          </document>
          )*"));
    }
  }

  SECTION("Entity Documents for Macros") {
    util::cpp_file header(R"(
      #define MACRO(a, b) 1337
      )");

      auto parsed = util::parsed_comments(header).add(header["MACRO"], R"(
        A macro.
        \param a The parameter a.
        \param b The parameter b.
        )");

      auto options = create_entity_document_transformer::create_entity_document_transformer_options{};
      options.document_name = "doc_MACRO";
      options.document_path = "doc_MACRO";
      options.filter = [&](const cppast::cpp_entity& entity) { return &entity == &header["MACRO"]; };

      const auto documents = create_entity_document_transformer{&parsed.entities, header, options}.transform();
      REQUIRE(documents.size() == 1);
      const auto document = documents.begin()->as<model::document>();

      CHECK(xml_generator::render(document) == unindent(R"*(
          <?xml version="1.0"?>
          <document name="doc_MACRO">
            <entity-documentation name="MACRO">
              <section name="Brief">
                <paragraph>A macro.</paragraph>
              </section>
              <section name="Parameters">
                <entity-documentation name="a">
                  <section name="Brief">
                    <paragraph>The parameter a.</paragraph>
                  </section>
                </entity-documentation>
                <entity-documentation name="b">
                  <section name="Brief">
                    <paragraph>The parameter b.</paragraph>
                  </section>
                </entity-documentation>
              </section>
            </entity-documentation>
          </document>
          )*"));
  }

  SECTION("Entity Documents for Enums") {
    SECTION("Entity Document for a Classic Enum") {
      util::cpp_file header(R"(
        enum ENUM {
          A,
          B,
        };
        )");

      auto parsed = util::parsed_comments(header).add(header["ENUM"], R"(
        An enum.
        )");

      auto options = create_entity_document_transformer::create_entity_document_transformer_options{};
      options.document_name = "doc_ENUM";
      options.document_path = "doc_ENUM";
      options.filter = [&](const cppast::cpp_entity& entity) { return &entity == &header["ENUM"]; };

      const auto documents = create_entity_document_transformer{&parsed.entities, header, options}.transform();
      REQUIRE(documents.size() == 1);
      const auto document = documents.begin()->as<model::document>();

      CHECK(xml_generator::render(document) == unindent(R"*(
        <?xml version="1.0"?>
        <document name="doc_ENUM">
          <entity-documentation name="ENUM">
            <section name="Brief">
              <paragraph>An enum.</paragraph>
            </section>
            <entity-documentation name="A" />
            <entity-documentation name="B" />
          </entity-documentation>
        </document>
        )*"));
    }

    SECTION("Entity Document for a Scoped Enum") {
      util::cpp_file header(R"(
        enum class ENUM {
          A,
          B,
        };
        )");

      auto parsed = util::parsed_comments(header).add(header["ENUM"], R"(
        An enum.
        )");

      auto options = create_entity_document_transformer::create_entity_document_transformer_options{};
      options.document_name = "doc_ENUM";
      options.document_path = "doc_ENUM";
      options.filter = [&](const cppast::cpp_entity& entity) { return &entity == &header["ENUM"]; };

      const auto documents = create_entity_document_transformer{&parsed.entities, header, options}.transform();
      REQUIRE(documents.size() == 1);
      const auto document = documents.begin()->as<model::document>();

      CHECK(xml_generator::render(document) == unindent(R"*(
        <?xml version="1.0"?>
        <document name="doc_ENUM">
          <entity-documentation name="ENUM">
            <section name="Brief">
              <paragraph>An enum.</paragraph>
            </section>
            <entity-documentation name="A" />
            <entity-documentation name="B" />
          </entity-documentation>
        </document>
        )*"));
    }

    SECTION("Entity Document for an enum Value") {
      util::cpp_file header(R"(
        enum class ENUM {
          A,
          B,
        };
        )");

      auto parsed = util::parsed_comments(header).add(header["ENUM.A"], R"(
        The A value of ENUM.
        )");

      auto options = create_entity_document_transformer::create_entity_document_transformer_options{};
      options.document_name = "doc_A";
      options.document_path = "doc_A";
      options.filter = [&](const cppast::cpp_entity& entity) { return &entity == &header["ENUM.A"]; };

      const auto documents = create_entity_document_transformer{&parsed.entities, header, options}.transform();
      REQUIRE(documents.size() == 1);
      const auto document = documents.begin()->as<model::document>();

      CHECK(xml_generator::render(document) == unindent(R"*(
        <?xml version="1.0"?>
        <document name="doc_A">
          <entity-documentation name="A">
            <section name="Brief">
              <paragraph>The A value of ENUM.</paragraph>
            </section>
          </entity-documentation>
        </document>
        )*"));
    }
  }

  SECTION("Entity Documents for Usings") {
    SECTION("Entity Document for a Simple Alias") {
      util::cpp_file header(R"(
        using T = int;
        )");

      auto parsed = util::parsed_comments(header).add(header["T"], R"(
        An alias for int.
        )");

      auto options = create_entity_document_transformer::create_entity_document_transformer_options{};
      options.document_name = "doc_T";
      options.document_path = "doc_T";
      options.filter = [&](const cppast::cpp_entity& entity) { return &entity == &header["T"]; };

      const auto documents = create_entity_document_transformer{&parsed.entities, header, options}.transform();
      REQUIRE(documents.size() == 1);
      const auto document = documents.begin()->as<model::document>();

      CHECK(xml_generator::render(document) == unindent(R"*(
        <?xml version="1.0"?>
        <document name="doc_T">
          <entity-documentation name="T">
            <section name="Brief">
              <paragraph>An alias for int.</paragraph>
            </section>
          </entity-documentation>
        </document>
        )*"));
    }

    SECTION("Entity Document for a Template Using") {
      util::cpp_file header(R"(
        template <typename T>
        using U = int;
        )");

      auto parsed = util::parsed_comments(header).add(header["U"], R"(
        A template using.
        )");

      auto options = create_entity_document_transformer::create_entity_document_transformer_options{};
      options.document_name = "doc_U";
      options.document_path = "doc_U";
      options.filter = [&](const cppast::cpp_entity& entity) { return &entity == &header["U"]; };

      const auto documents = create_entity_document_transformer{&parsed.entities, header, options}.transform();
      REQUIRE(documents.size() == 1);
      const auto document = documents.begin()->as<model::document>();

      CHECK(xml_generator::render(document) == unindent(R"*(
        <?xml version="1.0"?>
        <document name="doc_U">
          <entity-documentation name="U">
            <section name="Brief">
              <paragraph>A template using.</paragraph>
            </section>
            <section name="Requires">
              <entity-documentation name="T" />
            </section>
          </entity-documentation>
        </document>
        )*"));
    }
  }
}

}
