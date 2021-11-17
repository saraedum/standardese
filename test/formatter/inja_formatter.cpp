// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <nlohmann/json.hpp>
#include <cppast/cpp_function.hpp>

#include "../../standardese/formatter/inja_formatter.hpp"
#include "../../standardese/model/cpp_entity_documentation.hpp"
#include "../../standardese/model/entity.hpp"
#include "../../standardese/model/module.hpp"
#include "../../standardese/model/document.hpp"
#include "../../standardese/model/markup/paragraph.hpp"
#include "../../standardese/output_generator/xml/xml_generator.hpp"
#include "../util/logger.hpp"
#include "../util/cpp_file.hpp"
#include "../util/unindent.hpp"

#include "../external/catch/single_include/catch2/catch.hpp"

namespace standardese::test::formatter {

using standardese::formatter::inja_formatter;

// TODO(0.6.0-beta): Test all the callbacks, directly and in a template.

TEST_CASE("Strings from Inja Templates", "[inja_formatter]") {
  auto logger = util::logger::throwing_logger();

  SECTION("`filename`, `absolute`, and `relative` Callbacks") {
    util::cpp_file header;
    auto inja = inja_formatter({}, header);

    SECTION("`absolute` Provides the Path of the Defining Header File") {
      inja.data().merge_patch(inja.to_json(header));

      REQUIRE(inja.format("{{ absolute }}") == header.path());

      SECTION("`filename` extracts the Filename Part") {
        REQUIRE(inja.format("{{ filename(absolute) }}") == header.path().filename());
      }
    }

    SECTION("`absolute` Cannot be used Without an Appropriate Context") {
      REQUIRE_THROWS(inja.format("{{ absolute }}"));
    }

    SECTION("`relative` Relativizes Paths") {
      REQUIRE(inja.relative(header, {inja.absolute(header)}) == inja.filename(inja.absolute(header)));

      REQUIRE(inja.relative(header, {"/unrelated/x", "/unrelated/y"}) == inja.absolute(header));
    }
  }

  SECTION("`name` Callback") {
    util::cpp_file header(util::unindent(R"(
      void f();

      class X {};
    )"));
    auto inja = inja_formatter({}, header);

    SECTION("`name` of a C++ Entity Provides the (Shortened) cppast Name of the Entity") {
      inja.data().merge_patch(inja.to_json(header));

      REQUIRE(inja.format("{{ name }}") == "header.hpp");
    }

    SECTION("`name` of a Module Provides the Module Name") {
      model::module context{"module"};

      inja.data().merge_patch(inja.to_json(context));

      REQUIRE(inja.format("{{ name }}") == context.name);
    }

    SECTION("`name` cannot be used Without an Appropriate Context") {
      REQUIRE_THROWS(inja.format("{{ name }}"));
    }

    // TODO(0.6.0-beta): Test all the other name() overloads.

    SECTION("`name` can be used as Advertised in the Documentation of to_json(cpp_entity)") {
      inja.data()["entity"] = inja.to_json(&header["f"]);

      REQUIRE(inja.format("{{ name(entity) }}") == "f");
    }

    SECTION("`name` can be used as Advertised in the Documentation of to_json(cpp_type)") {
      inja.data()["type"] = inja.to_json(&header["X"]);

      REQUIRE(inja.format("{{ name(type) }}") == "X");
    }
  }

  SECTION("`sanitize_basename` Callback") {
    util::cpp_file header;
    auto inja = inja_formatter({}, header);

    SECTION("`sanitize_basename` Replaces Strange Characters") {
      REQUIRE(inja.format(R"({{ sanitize_basename("a%b@ c") }})") == "a_b_c");
    }

    SECTION("`sanitize_basename` Replaces Characters that Should not be in a Base Name") {
      REQUIRE(inja.format(R"({{ sanitize_basename("...hpp") }})") == "_hpp");
    }
  }

  SECTION("`md_escape` Callback") {
    util::cpp_file header;
    auto inja = inja_formatter({}, header);

    REQUIRE(inja.format(R"({{ md_escape("`code`") }})") == R"(\`code\`)");
    REQUIRE(inja.format(R"({{ md_escape("A`B`C") }})") == R"(A\`B\`C)");
  }

  SECTION("`cppast_kind` Callback") {
    SECTION("`cppast_kind` of a File") {
      util::cpp_file header;
      auto inja = inja_formatter({}, header);

      inja.data().merge_patch(inja.to_json(header));

      REQUIRE(inja.cppast_kind(header) == "file");
      REQUIRE(inja.format("{{ cppast_kind }}") == "file");
    }

    SECTION("For a cpp_type") {
      util::cpp_file header(R"(void f();)");
      auto inja = inja_formatter({}, header);

      REQUIRE(inja.cppast_kind(static_cast<const cppast::cpp_function&>(header["f"]).return_type()) == "builtin");

      inja.data() = inja.to_json(&header["f"]);
      REQUIRE(inja.format(R"({{ cppast_kind(return_type) }})") == "builtin");
    }
  }

  SECTION("`scope` Callback") {
    SECTION("`scope` of a Template Declaration") {
      util::cpp_file header(R"(
        class A {
          template <typename T>
          class C {
          };
        };
      )");

      auto inja = inja_formatter({}, header);

      REQUIRE(inja.scope(header["A::C"]) == "A");
    }

    SECTION("`scope` of a Template Instantiation Returns the Scope of the Primary Declaration") {
      util::cpp_file header(R"(
        struct A {
          template <typename T>
          class C {};
        };

        void f(A::C<int> x);
      )");

      auto inja = inja_formatter({}, header);
      // Get the type of the parameter x of f.
      const auto& C = inja.type(*inja.parameters(header["f"]).at(0));
      REQUIRE(inja.scope(C) == "A");
    }

    // TODO(0.6.0-beta): Test all other entities and types.
  }

  SECTION("`namespace` Callback") {
    SECTION("`namespace` of a Template Declaration") {
      util::cpp_file header(R"(
        namespace A {
          template <typename T>
          class C {
          };
        };
      )");

      auto inja = inja_formatter({}, header);

      REQUIRE(inja.namespaze(header["A::C"]) == "A");
    }

    SECTION("`namespace` of a Template Instantiation Returns the Namespace of the Primary Declaration") {
      util::cpp_file header(R"(
        namespace A {
          template <typename T>
          class C {};
        };

        void f(A::C<int> x);
      )");

      auto inja = inja_formatter({}, header);
      // Get the type of the parameter x of f.
      const auto& C = inja.type(*inja.parameters(header["f"]).at(0));
      REQUIRE(inja.namespaze(C) == "A");
    }

    // TODO(0.6.0-beta): Test all other entities and types.
  }

  SECTION("`md` Callback") {
    SECTION("`md` can Render an Empty Document") {
      util::cpp_file header;

      auto inja = inja_formatter({}, header);
      auto document = model::document::container();

      SECTION("Directly Invoking the Command") {
        REQUIRE(inja.md(document) == "\n");
      }

      SECTION("In a Template") {
        inja.data().merge_patch(inja.to_json(document));
        REQUIRE(inja.format(R"({{ md }})") == "\n");
      }
    }

    SECTION("`md` can Render an Empty Paragraph") {
      util::cpp_file header;

      auto inja = inja_formatter({}, header);
      REQUIRE(inja.md(model::markup::paragraph{}) == "\n");
    }

    SECTION("`md` can Render a Code Block") {
      // TODO(0.6.0-alpha): Implement me.
    }

    SECTION("`md` can Render Code") {
      // TODO(0.6.0-alpha): Implement me.
    }

    SECTION("`md` can Render a Paragraph") {
      // TODO(0.6.0-alpha): Implement me.
    }

    SECTION("`md` can Render Text") {
      // TODO(0.6.0-alpha): Implement me.
    }

    SECTION("`md` Renders the Output of `code` as MarkDown") {
      util::cpp_file header;
      auto inja = inja_formatter({}, header);

      // Note that the output has a trailing newline since code() creates a paragraph.
      REQUIRE(inja.md(inja.code("void f()")) == R"(`void f()`)" "\n");
    }
  }

  SECTION("`text` Callback") {
    SECTION("`text` Renders Code as Plain Text") {
      util::cpp_file header(R"(
        struct A;

        bool operator==(const A&, const A&);
      )");

      auto inja = inja_formatter({}, header);

      // Note that the output has a trailing newline since code() creates a paragraph.
      REQUIRE(inja.text(inja.code(R"(bool operator==(const A&, const A&))")) == R"(bool operator==(const A&, const A&))" "\n");
    }
  }

  SECTION("`replace` Callback") {
    SECTION("Replaces all Matches of a Regular Expression") {
      util::cpp_file header;
      auto inja = inja_formatter({}, header);
      
      REQUIRE(inja.replace("mp_limb_signed_t", "mp_limb_signed_t", "slong") == "slong");
      REQUIRE(inja.replace(R"(mp\_limb\_signed\_t)", R"(mp\\?_limb\\?_signed\\?_t)", "slong") == "slong");
    }
  }

  SECTION("`error` Callback") {
    SECTION("Invoke Callback with a Simple String Argument") {
      util::cpp_file header;
      auto inja = inja_formatter({}, header);

      REQUIRE_THROWS_AS(inja.error("error message"), test::util::logger::message_logged_error);
      REQUIRE_THROWS_AS(inja.format(R"({{ error("error message") }})"), test::util::logger::message_logged_error);
    }
    
    // A more realistic use case:
    SECTION("Invoke Callback with a Formatted String Argument") {
      util::cpp_file header;
      auto inja = inja_formatter({}, header);
      inja.data().merge_patch(inja.to_json(header));

      std::stringstream s;
      util::logger::capturing_logger(s);

      REQUIRE(inja.format(R"({{ error(format("failed to process header {{ name }}")) }})") == "");

      CAPTURE(s.str());
      REQUIRE(s.str().find("failed to process header header.hpp") != std::string::npos);
    }
  }

  SECTION("`warn` Callback") {
    util::cpp_file header;
    auto inja = inja_formatter({}, header);

    REQUIRE_THROWS_AS(inja.warn("warning message"), test::util::logger::message_logged_error);
    REQUIRE_THROWS_AS(inja.format(R"({{ warn("warning message") }})"), test::util::logger::message_logged_error);
  }

  SECTION("`info` Callback") {
    util::cpp_file header;
    auto inja = inja_formatter({}, header);
    std::stringstream s;
    util::logger::capturing_logger(s);

    SECTION("Invoked Directly") {
      inja.info("informational message");

      CAPTURE(s.str());
      REQUIRE(s.str().find("informational message") != std::string::npos);
    }

    SECTION("Invoked through Template") {
      REQUIRE(inja.format(R"({{ info("informational message") }})") == "");

      CAPTURE(s.str());
      REQUIRE(s.str().find("informational message") != std::string::npos);
    }
  }

  SECTION("`debug` Callback") {
    util::cpp_file header;
    auto inja = inja_formatter({}, header);
    std::stringstream s;
    util::logger::capturing_logger(s);

    SECTION("Invoked Directly") {
      inja.debug("debug message");

      // Debug messages are not logged normally.
      REQUIRE(s.str() == "");
    }

    SECTION("Invoked through Template") {
      REQUIRE(inja.format(R"({{ debug("debug message") }})") == "");

      // Debug messages are not logged normally.
      REQUIRE(s.str() == "");
    }
  }

  SECTION("`trace` Callback") {
    util::cpp_file header;
    auto inja = inja_formatter({}, header);
    std::stringstream s;
    util::logger::capturing_logger(s);

    SECTION("Invoked Directly") {
      inja.trace("trace message");

      // Trace messages are not logged normally.
      REQUIRE(s.str() == "");
    }

    SECTION("Invoked through Template") {
      REQUIRE(inja.format(R"({{ trace("trace message") }})") == "");

      // Trace messages are not logged normally.
      REQUIRE(s.str() == "");
    }
  }
}

TEST_CASE("Markup Entities from Inja Templates", "[inja_formatter]") {
  auto logger = util::logger::throwing_logger();
  util::cpp_file header;
  auto inja = inja_formatter({}, header);

  SECTION("MarkDown is Supported") {
    const auto node = inja.parse("`code`");
    REQUIRE(output_generator::xml::xml_generator::render(node) == util::unindent(R"(
      <?xml version="1.0"?>
      <document>
        <paragraph>
          <code>code</code>
        </paragraph>
      </document>
      )"));
  }

  SECTION("MarkDown Can be Escaped") {
    const auto node = inja.parse(inja.format(R"({{ md_escape("`code`") }})"));
    REQUIRE(output_generator::xml::xml_generator::render(node) == util::unindent(R"(
      <?xml version="1.0"?>
      <document>
        <paragraph>`code`</paragraph>
      </document>
      )"));
  }
}

}
