// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include "../../standardese/formatter/code_formatter.hpp"
#include "../../standardese/model/cpp_entity_documentation.hpp"
#include "../../standardese/model/document.hpp"
#include "../../standardese/output_generator/xml/xml_generator.hpp"

#include "../../external/catch/single_include/catch2/catch.hpp"

#include "../util/cpp_file.hpp"
#include "../util/parsed_comments.hpp"
#include "../util/unindent.hpp"

namespace standardese::test::formatter {

using standardese::formatter::code_formatter;
using output_generator::xml::xml_generator;

TEST_CASE("Functions can be Formatted", "[code_formatter]") {
  // https://en.cppreference.com/w/cpp/language/types
  SECTION("Fundamental Return Types") {
    SECTION("void") {
      util::cpp_file header(R"(void f();)");
      auto formatted = code_formatter{{}}.build(
        header["f"],
        model::cpp_entity_documentation(header, header));

      REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
        <?xml version="1.0"?>
        <document>
          <paragraph>
            <code>void f()</code>
          </paragraph>
        </document>
        )"));
    }

    SECTION("nullptr_t") {
      util::cpp_file header(R"(
        #include <cstddef>

        // Has no effect on the printing of nullptr_t below.
        using std::nullptr_t;

        nullptr_t f();
      )");

      SECTION("With std namespace") {
        auto formatted = code_formatter{{}}.build(
          header["f"],
          model::cpp_entity_documentation(header, header));

        REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
          <?xml version="1.0"?>
          <document>
            <paragraph>
              <code>std::nullptr_t f()</code>
            </paragraph>
          </document>
          )"));
      }

      SECTION("Omitting all namespaces") {
        code_formatter::code_formatter_options options;
        options.namespace_display_options = formatter::code_formatter::code_formatter_options::namespace_display_options::hidden;

        auto formatted = code_formatter{options}.build(
          header["f"],
          model::cpp_entity_documentation(header, header));

        REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
          <?xml version="1.0"?>
          <document>
            <paragraph>
              <code>nullptr_t f()</code>
            </paragraph>
          </document>
          )"));
      }
    }

    SECTION("Integer Types") {
      SECTION("int") {
        util::cpp_file header(R"(int f();)");
        auto formatted = code_formatter{{}}.build(
          header["f"],
          model::cpp_entity_documentation(header, header));

        REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
          <?xml version="1.0"?>
          <document>
            <paragraph>
              <code>int f()</code>
            </paragraph>
          </document>
          )"));
      }

      SECTION("unsigned long long") {
        util::cpp_file header(R"(unsigned long long f();)");
        auto formatted = code_formatter{{}}.build(
          header["f"],
          model::cpp_entity_documentation(header, header));

        REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
          <?xml version="1.0"?>
          <document>
            <paragraph>
              <code>unsigned long long f()</code>
            </paragraph>
          </document>
          )"));
      }

      SECTION("Some Trivial Modifiers are Dropped") {
        util::cpp_file header(R"(signed long long int f();)");
        auto formatted = code_formatter{{}}.build(
          header["f"],
          model::cpp_entity_documentation(header, header));

        REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
          <?xml version="1.0"?>
          <document>
            <paragraph>
              <code>long long f()</code>
            </paragraph>
          </document>
          )"));
      }
    }

    SECTION("bool") {
      util::cpp_file header(R"(bool f();)");
      auto formatted = code_formatter{{}}.build(
        header["f"],
        model::cpp_entity_documentation(header, header));

      REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
        <?xml version="1.0"?>
        <document>
          <paragraph>
            <code>bool f()</code>
          </paragraph>
        </document>
        )"));
    }

    SECTION("Float Pointing Types") {
      SECTION("float") {
        util::cpp_file header(R"(float f();)");
        auto formatted = code_formatter{{}}.build(
          header["f"],
          model::cpp_entity_documentation(header, header));

        REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
          <?xml version="1.0"?>
          <document>
            <paragraph>
              <code>float f()</code>
            </paragraph>
          </document>
          )"));
      }

      SECTION("double") {
        util::cpp_file header(R"(double f();)");
        auto formatted = code_formatter{{}}.build(
          header["f"],
          model::cpp_entity_documentation(header, header));

        REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
          <?xml version="1.0"?>
          <document>
            <paragraph>
              <code>double f()</code>
            </paragraph>
          </document>
          )"));
      }
    }
  }

  SECTION("Pointer Types") {
    SECTION("Pointer to Builtin Type") {
      util::cpp_file header(R"(void* f();)");
      auto formatted = code_formatter{{}}.build(
        header["f"],
        model::cpp_entity_documentation(header, header));

      REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
        <?xml version="1.0"?>
        <document>
          <paragraph>
            <code>void* f()</code>
          </paragraph>
        </document>
        )"));
    }

    SECTION("Pointer to Unexposed Type") {
      util::cpp_file header(R"(
        #include <string>

        std::string* f();
      )");
      auto formatted = code_formatter{{}}.build(
        header["f"],
        model::cpp_entity_documentation(header, header));

      REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
        <?xml version="1.0"?>
        <document>
          <paragraph>
            <code>std::string* f()</code>
          </paragraph>
        </document>
        )"));
    }

    SECTION("Pointer to User-Defined Type") {
      util::cpp_file header(R"(
        class X {};

        X* f();
      )");
      auto formatted = code_formatter{{}}.build(
        header["f"],
        model::cpp_entity_documentation(header, header));

      REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
        <?xml version="1.0"?>
        <document>
          <paragraph>
            <code>X* f()</code>
          </paragraph>
        </document>
        )"));
    }
  }

  SECTION("Reference Types") {
    SECTION("Pointer to Builtin Type") {
      util::cpp_file header(R"(int& f();)");
      auto formatted = code_formatter{{}}.build(
        header["f"],
        model::cpp_entity_documentation(header, header));

      REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
        <?xml version="1.0"?>
        <document>
          <paragraph>
            <code>int&amp; f()</code>
          </paragraph>
        </document>
        )"));
    }

    SECTION("Reference to Unexposed Type") {
      util::cpp_file header(R"(
        #include <string>

        const std::string& f();
      )");
      auto formatted = code_formatter{{}}.build(
        header["f"],
        model::cpp_entity_documentation(header, header));

      REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
        <?xml version="1.0"?>
        <document>
          <paragraph>
            <code>const std::string&amp; f()</code>
          </paragraph>
        </document>
        )"));
    }

    SECTION("Reference to User-Defined Type") {
      util::cpp_file header(R"(
        class X {};

        X& f();
      )");
      auto formatted = code_formatter{{}}.build(
        header["f"],
        model::cpp_entity_documentation(header, header));

      REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
        <?xml version="1.0"?>
        <document>
          <paragraph>
            <code>X&amp; f()</code>
          </paragraph>
        </document>
        )"));
    }
  }

  SECTION("CV Qualified Types") {
    // TODO
  }

  SECTION("Template Instantiation Type") {
    SECTION("Instantiation of Templates not Defined in the Parsed Code") {
      util::cpp_file header(R"(
        #include <vector>

        std::vector<int> f();
      )");

      auto formatted = code_formatter{{}}.build(
        header["f"],
        model::cpp_entity_documentation(header, header));

      REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
        <?xml version="1.0"?>
        <document>
          <paragraph>
            <code>std::vector&lt;int&gt; f()</code>
          </paragraph>
        </document>
        )"));
    }
    // TODO
  }

  /*
  SECTION("Internal User Defined Return Types") {
    util::cpp_file header(R"(
      class C {
        C clone();
      };
      )");

    auto formatted = code_formatter{{}}.build(
        header["C::clone"],
        model::cpp_entity_documentation(header, header));

    REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
      <?xml version="1.0"?>
      <document>
        <paragraph>
          <link target-entity="C">
            <code>C</code>
          </link> clone()</paragraph>
      </document>      
      )"));
  }

  SECTION("External User Defined Return Types") {
  }
  */
}

}
