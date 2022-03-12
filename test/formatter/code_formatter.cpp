// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include "../../standardese/formatter/code_formatter.hpp"
#include "../../standardese/model/cpp_entity_documentation.hpp"
#include "../../standardese/model/document.hpp"
#include "../../standardese/model/markup/paragraph.hpp"
#include "../../standardese/output_generator/xml/xml_generator.hpp"

#include "../external/catch/single_include/catch2/catch.hpp"

#include "../util/cpp_file.hpp"
#include "../util/parsed_comments.hpp"
#include "../util/unindent.hpp"
#include "../util/logger.hpp"

namespace standardese::test::formatter {

using standardese::formatter::code_formatter;
using output_generator::xml::xml_generator;

TEST_CASE("Functions can be Formatted", "[code_formatter]") {
  auto logger = util::logger::throwing_logger();

  SECTION("Top-Level Functions") {
    SECTION("Non-Template Functions") {
      util::cpp_file header(R"(void f();)");
      auto formatted = code_formatter{{}, header}.build(header["f"], header);

      REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
        <?xml version="1.0"?>
        <paragraph>
          <code>void f()</code>
        </paragraph>
        )"));
    }

    SECTION("Template Functions") {
      SECTION("Template Functions with Non-Type Argument") {
        util::cpp_file header(util::unindent(R"(
          template <int x>
          void f();
        )"));

        auto formatted = code_formatter{{}, header}.build(header["f"], header);

        REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
          <?xml version="1.0"?>
          <paragraph>
            <code>template&lt;int x&gt; void f()</code>
          </paragraph>
          )"));
      }

      SECTION("Template Functions with Unnamed Non-Type Argument") {
        util::cpp_file header(util::unindent(R"(
          template <int>
          void f();
        )"));

        auto formatted = code_formatter{{}, header}.build(header["f"], header);

        REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
          <?xml version="1.0"?>
          <paragraph>
            <code>template&lt;int&gt; void f()</code>
          </paragraph>
          )"));
      }

      SECTION("Template Functions with Non-Type Parameter Pack Argument") {
        util::cpp_file header(util::unindent(R"(
          template <int... x>
          void f();
        )"));

        auto formatted = code_formatter{{}, header}.build(header["f"], header);

        REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
          <?xml version="1.0"?>
          <paragraph>
            <code>template&lt;int... x&gt; void f()</code>
          </paragraph>
          )"));
      }

      SECTION("Template Functions with Non-Type Placeholder Argument") {
        util::cpp_file header(util::unindent(R"(
          template <auto** x>
          void f();
        )"));

        auto formatted = code_formatter{{}, header}.build(header["f"], header);

        REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
          <?xml version="1.0"?>
          <paragraph>
            <code>template&lt;auto** x&gt; void f()</code>
          </paragraph>
          )"));
      }

      SECTION("Template Functions with Unnamed Non-Type Placeholder Parameter Pack Argument") {
        util::cpp_file header(util::unindent(R"(
          template <auto...>
          void f();
        )"));

        auto formatted = code_formatter{{}, header}.build(header["f"], header);

        REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
          <?xml version="1.0"?>
          <paragraph>
            <code>template&lt;auto...&gt; void f()</code>
          </paragraph>
          )"));
      }

      SECTION("Template Functions with Type Argument") {
        util::cpp_file header(util::unindent(R"(
          template <typename T>
          void f();
        )"));

        auto formatted = code_formatter{{}, header}.build(header["f"], header);

        REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
          <?xml version="1.0"?>
          <paragraph>
            <code>template&lt;typename T&gt; void f()</code>
          </paragraph>
          )"));
      }

      SECTION("Template Functions with Unnamed Type Argument") {
        util::cpp_file header(util::unindent(R"(
          template <typename>
          void f();
        )"));

        auto formatted = code_formatter{{}, header}.build(header["f"], header);

        REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
          <?xml version="1.0"?>
          <paragraph>
            <code>template&lt;typename&gt; void f()</code>
          </paragraph>
          )"));
      }

      SECTION("Template Functions with Parameter Pack Type Argument") {
        util::cpp_file header(util::unindent(R"(
          template <typename... T>
          void f();
        )"));

        auto formatted = code_formatter{{}, header}.build(header["f"], header);

        REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
          <?xml version="1.0"?>
          <paragraph>
            <code>template&lt;typename... T&gt; void f()</code>
          </paragraph>
          )"));
      }

      SECTION("Template Functions with Unnamed Parameter Pack Type Argument") {
        util::cpp_file header(util::unindent(R"(
          template <typename...>
          void f();
        )"));

        auto formatted = code_formatter{{}, header}.build(header["f"], header);

        REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
          <?xml version="1.0"?>
          <paragraph>
            <code>template&lt;typename...&gt; void f()</code>
          </paragraph>
          )"));
      }

      SECTION("Template Functions with Template Template Argument") {
        util::cpp_file header(util::unindent(R"(
          template <template <class> typename T>
          void f();
        )"));

        auto formatted = code_formatter{{}, header}.build(header["f"], header);

        REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
          <?xml version="1.0"?>
          <paragraph>
            <code>template&lt;template&lt;typename&gt; typename T&gt; void f()</code>
          </paragraph>
          )"));
      }

      SECTION("Template Functions with Unnamed Template Template Argument") {
        util::cpp_file header(util::unindent(R"(
          template <template <typename, int> typename>
          void f();
        )"));

        auto formatted = code_formatter{{}, header}.build(header["f"], header);

        REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
          <?xml version="1.0"?>
          <paragraph>
            <code>template&lt;template&lt;typename, int&gt; typename&gt; void f()</code>
          </paragraph>
          )"));
      }

      SECTION("Template Functions with Template Template Parameter Pack Argument") {
        util::cpp_file header(util::unindent(R"(
          template <template <typename> typename... T>
          void f();
        )"));

        auto formatted = code_formatter{{}, header}.build(header["f"], header);

        REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
          <?xml version="1.0"?>
          <paragraph>
            <code>template&lt;template&lt;typename&gt; typename... T&gt; void f()</code>
          </paragraph>
          )"));
      }
    }
  }

  SECTION("Member Functions") {
    util::cpp_file header(R"(class C { void f(); };)");

    SECTION("In the Class Context") {
      auto formatted = code_formatter{{}, header}.build(header["C::f"], header["C"]);

      REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
        <?xml version="1.0"?>
        <paragraph>
          <code>void f()</code>
        </paragraph>
        )"));
    }

    SECTION("Outside the Class Context") {
      auto formatted = code_formatter{{}, header}.build(header["C::f"], header);

      REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
        <?xml version="1.0"?>
        <paragraph>
          <code>void C::f()</code>
        </paragraph>
        )"));
    }

    SECTION("Without any Context") {
      auto formatted = code_formatter{{}, header}.build(header["C::f"]);

      REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
        <?xml version="1.0"?>
        <paragraph>
          <code>void C::f()</code>
        </paragraph>
        )"));
    }
  }

  SECTION("Constructors") {
    util::cpp_file header(R"(class C { C(); };)");

    SECTION("In the Class Context") {
      auto formatted = code_formatter{{}, header}.build(header["C::C"], header["C"]);

      REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
        <?xml version="1.0"?>
        <paragraph>
          <code>C()</code>
        </paragraph>
        )"));
    }

    SECTION("Outside the Class Context") {
      auto formatted = code_formatter{{}, header}.build(header["C::C"], header);

      REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
        <?xml version="1.0"?>
        <paragraph>
          <code>C::C()</code>
        </paragraph>
        )"));
    }

    SECTION("Without any Context") {
      auto formatted = code_formatter{{}, header}.build(header["C::C"]);

      REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
        <?xml version="1.0"?>
        <paragraph>
          <code>C::C()</code>
        </paragraph>
        )"));
    }
  }

  SECTION("Destructors") {
    util::cpp_file header(R"(class C { ~C(); };)");

    SECTION("In the Class Context") {
      auto formatted = code_formatter{{}, header}.build(header["C::~C"], header["C"]);

      REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
        <?xml version="1.0"?>
        <paragraph>
          <code>~C()</code>
        </paragraph>
        )"));
    }

    SECTION("Outside the Class Context") {
      auto formatted = code_formatter{{}, header}.build(header["C::~C"], header);

      REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
        <?xml version="1.0"?>
        <paragraph>
          <code>C::~C()</code>
        </paragraph>
        )"));
    }

    SECTION("Without any Context") {
      auto formatted = code_formatter{{}, header}.build(header["C::~C"]);

      REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
        <?xml version="1.0"?>
        <paragraph>
          <code>C::~C()</code>
        </paragraph>
        )"));
    }
  }

  SECTION("Conversion Operators") {
    util::cpp_file header(R"(struct C { operator int(); };)");

    SECTION("In the Struct Context") {
      auto formatted = code_formatter{{}, header}.build(header["C::operator int"], header["C"]);

      REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
        <?xml version="1.0"?>
        <paragraph>
          <code>operator int()</code>
        </paragraph>
        )"));
    }

    SECTION("Outside the Struct Context") {
      auto formatted = code_formatter{{}, header}.build(header["C::operator int"], header);

      REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
        <?xml version="1.0"?>
        <paragraph>
          <code>C::operator int()</code>
        </paragraph>
        )"));
    }

    SECTION("Without any Context") {
      auto formatted = code_formatter{{}, header}.build(header["C::operator int"]);

      REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
        <?xml version="1.0"?>
        <paragraph>
          <code>C::operator int()</code>
        </paragraph>
        )"));
    }
  }

  SECTION("Operators") {
    util::cpp_file header(R"(class C { C& operator+=(const C&); };)");

    SECTION("In the Class Context") {
      auto formatted = code_formatter{{}, header}.build(header["C::operator+="], header["C"]);

      REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
        <?xml version="1.0"?>
        <paragraph>
          <link target-entity="C">
            <code>C</code>
          </link>
          <code>&amp; operator+=(const </code>
          <link target-entity="C">
            <code>C</code>
          </link>
          <code>&amp;)</code>
        </paragraph>
        )"));
    }

    SECTION("Outside the Class Context") {
      auto formatted = code_formatter{{}, header}.build(header["C::operator+="], header);

      REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
        <?xml version="1.0"?>
        <paragraph>
          <link target-entity="C">
            <code>C</code>
          </link>
          <code>&amp; C::operator+=(const </code>
          <link target-entity="C">
            <code>C</code>
          </link>
          <code>&amp;)</code>
        </paragraph>
        )"));
    }

    SECTION("Without any Context") {
      auto formatted = code_formatter{{}, header}.build(header["C::operator+="]);

      REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
        <?xml version="1.0"?>
        <paragraph>
          <link target-entity="C">
            <code>C</code>
          </link>
          <code>&amp; C::operator+=(const </code>
          <link target-entity="C">
            <code>C</code>
          </link>
          <code>&amp;)</code>
        </paragraph>
        )"));
    }
  }

  SECTION("Friends") {
    SECTION("Friend Operators") {
      util::cpp_file header(util::unindent(R"(
        #include <ostream>

        namespace A {
          class C { friend std::ostream& operator<<(std::ostream&, const C&); };
        }
      )"));

      SECTION("In the Class Context") {
        auto formatted = code_formatter{{}, header}.build(header["A::operator<<"], header["A::C"]);

        REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
          <?xml version="1.0"?>
          <paragraph>
            <code>std::ostream&amp; operator&lt;&lt;(std::ostream&amp;, const </code>
            <link target-entity="C">
              <code>C</code>
            </link>
            <code>&amp;)</code>
          </paragraph>
          )"));
      }

      SECTION("Outside the Class Context") {
        auto formatted = code_formatter{{}, header}.build(header["A::operator<<"], header);

        REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
          <?xml version="1.0"?>
          <paragraph>
            <code>std::ostream&amp; A::operator&lt;&lt;(std::ostream&amp;, const </code>
            <link target-entity="C">
              <code>A::C</code>
            </link>
            <code>&amp;)</code>
          </paragraph>
          )"));
      }

      SECTION("Without any Context") {
        auto formatted = code_formatter{{}, header}.build(header["A::operator<<"]);

        REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
          <?xml version="1.0"?>
          <paragraph>
            <code>std::ostream&amp; A::operator&lt;&lt;(std::ostream&amp;, const </code>
            <link target-entity="C">
              <code>A::C</code>
            </link>
            <code>&amp;)</code>
          </paragraph>
          )"));
      }
    }

    SECTION("Template Friend Operators") {
      util::cpp_file header(util::unindent(R"(
        #include <ostream>

        namespace A {
          template <typename T>
          class C {
            template <typename S>
            friend std::ostream& operator<<(std::ostream&, const C<S>&);
          };
        }
      )"));

      SECTION("In the Class Context") {
        auto formatted = code_formatter{{}, header}.build(header["A::operator<<"], header["A::C"]);

        REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
          <?xml version="1.0"?>
          <paragraph>
            <code>template&lt;typename S&gt; std::ostream&amp; operator&lt;&lt;(std::ostream&amp;, const </code>
            <link target-entity="C">
              <code>C&lt;S&gt;</code>
            </link>
            <code>&amp;)</code>
          </paragraph>
          )"));
      }

      SECTION("Outside the Class Context") {
        auto formatted = code_formatter{{}, header}.build(header["A::operator<<"], header);

        REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
          <?xml version="1.0"?>
          <paragraph>
            <code>template&lt;typename S&gt; std::ostream&amp; A::operator&lt;&lt;(std::ostream&amp;, const </code>
            <link target-entity="C">
              <code>A::C&lt;S&gt;</code>
            </link>
            <code>&amp;)</code>
          </paragraph>
          )"));
      }

      SECTION("Without any Context") {
        auto formatted = code_formatter{{}, header}.build(header["A::operator<<"]);

        REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
          <?xml version="1.0"?>
          <paragraph>
            <code>template&lt;typename S&gt; std::ostream&amp; A::operator&lt;&lt;(std::ostream&amp;, const </code>
            <link target-entity="C">
              <code>A::C&lt;S&gt;</code>
            </link>
            <code>&amp;)</code>
          </paragraph>
          )"));
      }
    }

    SECTION("Friend Functions") {
      util::cpp_file header(util::unindent(R"(
        #include <ostream>

        namespace A {
          class C {
            friend std::ostream& f(std::ostream&, const C&);
          };
        }
      )"));

      SECTION("In the Class Context") {
        auto formatted = code_formatter{{}, header}.build(header["A::f"], header["A::C"]);

        REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
          <?xml version="1.0"?>
          <paragraph>
            <code>std::ostream&amp; f(std::ostream&amp;, const </code>
            <link target-entity="C">
              <code>C</code>
            </link>
            <code>&amp;)</code>
          </paragraph>
          )"));
      }

      SECTION("Outside the Class Context") {
        auto formatted = code_formatter{{}, header}.build(header["A::f"], header);

        REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
          <?xml version="1.0"?>
          <paragraph>
            <code>std::ostream&amp; A::f(std::ostream&amp;, const </code>
            <link target-entity="C">
              <code>A::C</code>
            </link>
            <code>&amp;)</code>
          </paragraph>
          )"));
      }

      SECTION("Without any Context") {
        auto formatted = code_formatter{{}, header}.build(header["A::f"]);

        REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
          <?xml version="1.0"?>
          <paragraph>
            <code>std::ostream&amp; A::f(std::ostream&amp;, const </code>
            <link target-entity="C">
              <code>A::C</code>
            </link>
            <code>&amp;)</code>
          </paragraph>
          )"));
      }
    }

    SECTION("Template Friend Functions") {
      util::cpp_file header(util::unindent(R"(
        #include <ostream>

        namespace A {
          template <typename T>
          class C {
            template <typename S>
            friend std::ostream& f(std::ostream&, const C<S>&);
          };
        }
      )"));

      SECTION("In the Class Context") {
        auto formatted = code_formatter{{}, header}.build(header["A::f"], header["A::C"]);

        REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
          <?xml version="1.0"?>
          <paragraph>
            <code>template&lt;typename S&gt; std::ostream&amp; f(std::ostream&amp;, const </code>
            <link target-entity="C">
              <code>C&lt;S&gt;</code>
            </link>
            <code>&amp;)</code>
          </paragraph>
          )"));
      }

      SECTION("Outside the Class Context") {
        auto formatted = code_formatter{{}, header}.build(header["A::f"], header);

        REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
          <?xml version="1.0"?>
          <paragraph>
            <code>template&lt;typename S&gt; std::ostream&amp; A::f(std::ostream&amp;, const </code>
            <link target-entity="C">
              <code>A::C&lt;S&gt;</code>
            </link>
            <code>&amp;)</code>
          </paragraph>
          )"));
      }

      SECTION("Without any Context") {
        auto formatted = code_formatter{{}, header}.build(header["A::f"]);

        REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
          <?xml version="1.0"?>
          <paragraph>
            <code>template&lt;typename S&gt; std::ostream&amp; A::f(std::ostream&amp;, const </code>
            <link target-entity="C">
              <code>A::C&lt;S&gt;</code>
            </link>
            <code>&amp;)</code>
          </paragraph>
          )"));
      }
    }
  }

  // https://en.cppreference.com/w/cpp/language/types
  SECTION("Fundamental Return Types") {
    SECTION("void") {
      util::cpp_file header(R"(void f();)");
      auto formatted = code_formatter{{}, header}.build(header["f"]);

      REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
        <?xml version="1.0"?>
        <paragraph>
          <code>void f()</code>
        </paragraph>
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
        auto formatted = code_formatter{{}, header}.build(header["f"]);

        REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
          <?xml version="1.0"?>
          <paragraph>
            <code>std::nullptr_t f()</code>
          </paragraph>
          )"));
      }

      SECTION("Omitting all namespaces") {
        code_formatter::code_formatter_options options;
        options.namespace_display_options = formatter::code_formatter::code_formatter_options::namespace_display_options::hidden;

        auto formatted = code_formatter{options, header}.build(header["f"]);

        REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
          <?xml version="1.0"?>
          <paragraph>
            <code>nullptr_t f()</code>
          </paragraph>
          )"));
      }
    }

    SECTION("Integer Types") {
      SECTION("int") {
        util::cpp_file header(R"(int f();)");
        auto formatted = code_formatter{{}, header}.build(header["f"]);

        REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
          <?xml version="1.0"?>
          <paragraph>
            <code>int f()</code>
          </paragraph>
          )"));
      }

      SECTION("unsigned long long") {
        util::cpp_file header(R"(unsigned long long f();)");
        auto formatted = code_formatter{{}, header}.build(header["f"]);

        REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
          <?xml version="1.0"?>
          <paragraph>
            <code>unsigned long long f()</code>
          </paragraph>
          )"));
      }

      SECTION("Some Trivial Modifiers are Dropped") {
        util::cpp_file header(R"(signed long long int f();)");
        auto formatted = code_formatter{{}, header}.build(header["f"]);

        REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
          <?xml version="1.0"?>
          <paragraph>
            <code>long long f()</code>
          </paragraph>
          )"));
      }
    }

    SECTION("bool") {
      util::cpp_file header(R"(bool f();)");
      auto formatted = code_formatter{{}, header}.build(header["f"]);

      REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
        <?xml version="1.0"?>
        <paragraph>
          <code>bool f()</code>
        </paragraph>
        )"));
    }

    SECTION("Float Pointing Types") {
      SECTION("float") {
        util::cpp_file header(R"(float f();)");
        auto formatted = code_formatter{{}, header}.build(header["f"]);

        REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
          <?xml version="1.0"?>
          <paragraph>
            <code>float f()</code>
          </paragraph>
          )"));
      }

      SECTION("double") {
        util::cpp_file header(R"(double f();)");
        auto formatted = code_formatter{{}, header}.build(header["f"]);

        REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
          <?xml version="1.0"?>
          <paragraph>
            <code>double f()</code>
          </paragraph>
          )"));
      }
    }
  }

  SECTION("Pointer Return Types") {
    SECTION("Pointer to Builtin Type") {
      util::cpp_file header(R"(void* f();)");
      auto formatted = code_formatter{{}, header}.build(header["f"]);

      REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
        <?xml version="1.0"?>
        <paragraph>
          <code>void* f()</code>
        </paragraph>
        )"));
    }

    SECTION("Pointer to Unexposed Type") {
      util::cpp_file header(R"(
        #include <string>

        std::string* f();
      )");
      auto formatted = code_formatter{{}, header}.build(header["f"]);

      REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
        <?xml version="1.0"?>
        <paragraph>
          <code>std::string* f()</code>
        </paragraph>
        )"));
    }

    SECTION("Pointer to User-Defined Type") {
      util::cpp_file header(R"(
        class X {};

        X* f();
      )");
      auto formatted = code_formatter{{}, header}.build(header["f"]);

      REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
        <?xml version="1.0"?>
        <paragraph>
          <code>X* f()</code>
        </paragraph>
        )"));
    }
  }

  SECTION("Reference Return Types") {
    SECTION("Pointer to Builtin Type") {
      util::cpp_file header(R"(int& f();)");
      auto formatted = code_formatter{{}, header}.build(header["f"]);

      REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
        <?xml version="1.0"?>
        <paragraph>
          <code>int&amp; f()</code>
        </paragraph>
        )"));
    }

    SECTION("Reference to Unexposed Type") {
      util::cpp_file header(R"(
        #include <string>

        const std::string& f();
      )");
      auto formatted = code_formatter{{}, header}.build(header["f"]);

      REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
        <?xml version="1.0"?>
        <paragraph>
          <code>const std::string&amp; f()</code>
        </paragraph>
        )"));
    }

    SECTION("Reference to User-Defined Type") {
      util::cpp_file header(R"(
        class X {};

        X& f();
      )");
      auto formatted = code_formatter{{}, header}.build(header["f"]);

      REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
        <?xml version="1.0"?>
        <paragraph>
          <link target-entity="X">
            <code>X</code>
          </link>
          <code>&amp; f()</code>
        </paragraph>
        )"));
    }
  }

  SECTION("Template Instantiation Return Type") {
    SECTION("Instantiation of Templates not Defined in the Parsed Code") {
      util::cpp_file header(R"(
        #include <vector>

        std::vector<int> f();
      )");

      auto formatted = code_formatter{{}, header}.build(header["f"]);

      REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
        <?xml version="1.0"?>
        <paragraph>
          <code>std::vector&lt;int&gt; f()</code>
        </paragraph>
        )"));
    }

    SECTION("Instantiation of Templates Defined in the Parsed Code") {
      util::cpp_file header(R"(
        #include <vector>

        template <typename T>
        class X {};

        X<int> f();
      )");

      auto formatted = code_formatter{{}, header}.build(header["f"]);

      REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
        <?xml version="1.0"?>
        <paragraph>
          <link target-entity="X">
            <code>X&lt;int&gt;</code>
          </link>
          <code> f()</code>
        </paragraph>
        )"));
    }
  }

  SECTION("User Defined Return Types") {
    util::cpp_file header(R"(
      class C {
        C clone();
      };
      )");

    auto formatted = code_formatter{{}, header}.build(header["C::clone"]);

    REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
      <?xml version="1.0"?>
      <paragraph>
        <link target-entity="C">
          <code>C</code>
        </link>
        <code> C::clone()</code>
      </paragraph>
      )"));
  }

  SECTION("External User Defined Return Types") {
    util::cpp_file header(R"(
      #include <string>

      std::string f();
      )");

      auto formatted = code_formatter{{}, header}.build(header["f"]);

    REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
      <?xml version="1.0"?>
      <paragraph>
        <code>std::string f()</code>
      </paragraph>
      )"));
  }
}

TEST_CASE("Variables can be Formatted", "[code_formatter]") {
  SECTION("CV Qualified Types") {
    SECTION("Const Type") {
      util::cpp_file header(R"(const int a = 0;)");
      auto formatted = code_formatter{{}, header}.build(header["a"]);

      REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
        <?xml version="1.0"?>
        <paragraph>
          <code>const int a</code>
        </paragraph>
        )"));
    }

    SECTION("Volatile Type") {
      util::cpp_file header(R"(volatile int a;)");
      auto formatted = code_formatter{{}, header}.build(header["a"]);

      REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
        <?xml version="1.0"?>
        <paragraph>
          <code>volatile int a</code>
        </paragraph>
        )"));
    }

    SECTION("Const Volatile Type") {
      util::cpp_file header(R"(const volatile int a = 0;)");
      auto formatted = code_formatter{{}, header}.build(header["a"]);

      REQUIRE(xml_generator::render(formatted) == util::unindent(R"(
        <?xml version="1.0"?>
        <paragraph>
          <code>const volatile int a</code>
        </paragraph>
        )"));
    }
  }

  // TODO(0.6.0-beta): Other Variable Types
}

// TODO(0.6.0-beta): Formatting of Namespaces

// TODO(0.6.0-beta): Formatting of Classes Without Members

// TODO(0.6.0-beta): Formatting of Classis With Members

}
