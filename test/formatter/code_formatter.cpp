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
      auto formatted = code_formatter{{}, header}.build(
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
        auto formatted = code_formatter{{}, header}.build(
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

        auto formatted = code_formatter{options, header}.build(
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
        auto formatted = code_formatter{{}, header}.build(
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
        auto formatted = code_formatter{{}, header}.build(
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
        auto formatted = code_formatter{{}, header}.build(
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
      auto formatted = code_formatter{{}, header}.build(
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
        auto formatted = code_formatter{{}, header}.build(
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
        auto formatted = code_formatter{{}, header}.build(
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

  SECTION("Pointer Return Types") {
    SECTION("Pointer to Builtin Type") {
      util::cpp_file header(R"(void* f();)");
      auto formatted = code_formatter{{}, header}.build(
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
      auto formatted = code_formatter{{}, header}.build(
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
      auto formatted = code_formatter{{}, header}.build(
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

  SECTION("Reference Return Types") {
    SECTION("Pointer to Builtin Type") {
      util::cpp_file header(R"(int& f();)");
      auto formatted = code_formatter{{}, header}.build(
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
      auto formatted = code_formatter{{}, header}.build(
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
      auto formatted = code_formatter{{}, header}.build(
        header["f"],
        model::cpp_entity_documentation(header, header));

      REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
        <?xml version="1.0"?>
        <document>
          <paragraph>
            <link target-entity="X"> <code>X</code> </link> <code>&amp; f()</code>
          </paragraph>
        </document>
        )"));
    }
  }

  SECTION("Template Instantiation Return Type") {
    SECTION("Instantiation of Templates not Defined in the Parsed Code") {
      util::cpp_file header(R"(
        #include <vector>

        std::vector<int> f();
      )");

      auto formatted = code_formatter{{}, header}.build(
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

    SECTION("Instantiation of Templates Defined in the Parsed Code") {
      util::cpp_file header(R"(
        #include <vector>

        template <typename T>
        class X {};

        X<int> f();
      )");

      auto formatted = code_formatter{{}, header}.build(
        header["f"],
        model::cpp_entity_documentation(header, header));

      REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
        <?xml version="1.0"?>
        <document>
          <paragraph>
            <code>X&lt;int&gt; f()</code>
          </paragraph>
        </document>
        )"));
    }
  }

  SECTION("User Defined Return Types") {
    util::cpp_file header(R"(
      class C {
        C clone();
      };
      )");

    auto formatted = code_formatter{{}, header}.build(
        header["C::clone"],
        model::cpp_entity_documentation(header, header));

    REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
      <?xml version="1.0"?>
      <document>
        <paragraph>
          <link target-entity="C"> <code>C</code> </link> <code>clone()</code>
        </paragraph>
      </document>
      )"));
  }

  SECTION("External User Defined Return Types") {
    util::cpp_file header(R"(
      #include <string>

      std::string f();
      )");

      auto formatted = code_formatter{{}, header}.build(
        header["f"],
        model::cpp_entity_documentation(header, header));

    REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
      <?xml version="1.0"?>
      <document>
        <paragraph>
          <code>std::string f()</code>
        </paragraph>
      </document>
      )"));
  }
}

TEST_CASE("Variables can be Formatted", "[code_formatter]") {
  SECTION("CV Qualified Types") {
    SECTION("Const Type") {
      util::cpp_file header(R"(const int a = 0;)");
      auto formatted = code_formatter{{}, header}.build(
        header["a"],
        model::cpp_entity_documentation(header, header));

      REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
        <?xml version="1.0"?>
        <document>
          <paragraph>
            <code>const int a</code>
          </paragraph>
        </document>
        )"));
    }

    SECTION("Volatile Type") {
      util::cpp_file header(R"(volatile int a;)");
      auto formatted = code_formatter{{}, header}.build(
        header["a"],
        model::cpp_entity_documentation(header, header));

      REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
        <?xml version="1.0"?>
        <document>
          <paragraph>
            <code>volatile int a</code>
          </paragraph>
        </document>
        )"));
    }

    SECTION("Const Volatile Type") {
      util::cpp_file header(R"(const volatile int a = 0;)");
      auto formatted = code_formatter{{}, header}.build(
        header["a"],
        model::cpp_entity_documentation(header, header));

      REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
        <?xml version="1.0"?>
        <document>
          <paragraph>
            <code>const volatile int a</code>
          </paragraph>
        </document>
        )"));
    }
  }
}

}
