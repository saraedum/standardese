// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include "../../standardese/formatter/inja_formatter.hpp"
#include "../../standardese/model/cpp_entity_documentation.hpp"
#include "../../standardese/model/entity.hpp"
#include "../../standardese/model/module.hpp"
#include "../../standardese/model/document.hpp"
#include "../../standardese/output_generator/xml/xml_generator.hpp"
#include "../util/logger.hpp"
#include "../util/cpp_file.hpp"
#include "../util/unindent.hpp"

#include "../../external/catch/single_include/catch2/catch.hpp"

namespace standardese::test::formatter {

using standardese::formatter::inja_formatter;

TEST_CASE("Strings from Inja Templates", "[inja_formatter]") {
  auto logger = util::logger::throwing_logger();
  auto formatter = inja_formatter({});

  SECTION("`filename()` and `path` Callbacks") {
    SECTION("`path` Provides the Path of the Defining Header File") {
      util::cpp_file header;
      const auto context = model::cpp_entity_documentation{header, header};

      REQUIRE(formatter.format("{{ path }}", context) == header.path());

      SECTION("`filename` extracts the Filename Part") {
        REQUIRE(formatter.format("{{ filename(path) }}", context) == header.path().filename());
      }
    }

    SECTION("`path` Cannot be used Without an Appropriate Context") {
      REQUIRE_THROWS(formatter.format("{{ path }}", {}));
    }
  }

  SECTION("`name` Callback") {
    SECTION("`name` of a C++ Entity Provides the (Shortened) cppast Name of the Entity") {
      util::cpp_file header;
      const auto context = model::cpp_entity_documentation{header, header};

      REQUIRE(formatter.format("{{ name }}", context) == "header.hpp");
      REQUIRE(formatter.format("{{ name }}", context) == formatter.format(R"({{ name("short") }})", context));
    }

    SECTION("`name` of a Module Provides the Module Name") {
      model::module context{"module"};

      REQUIRE(formatter.format("{{ name }}", context) == context.name);
    }

    SECTION("`name` Cannot be used Without an Appropriate Context") {
      REQUIRE_THROWS(formatter.format("{{ name }}", {}));
    }
  }

  SECTION("`sanitize_basename` Callback") {
    SECTION("`sanitize_basename` Replaces Strange Characters") {
      REQUIRE(formatter.format(R"({{ sanitize_basename("a%b@ c") }})", {}) == "a_b_c");
    }

    SECTION("`sanitize_basename` Replaces Characters that Should not be in a Base Name") {
      REQUIRE(formatter.format(R"({{ sanitize_basename("...hpp") }})", {}) == "_hpp");
    }
  }

  SECTION("`md_escape Callback") {
    REQUIRE(formatter.format(R"({{ md_escape("`code`") }})", {}) == R"(\`code\`)");
    REQUIRE(formatter.format(R"({{ md_escape("A`B`C") }})", {}) == R"(A\`B\`C)");
  }
}

TEST_CASE("Markup Entities from Inja Templates", "[inja_formatter]") {
  auto logger = util::logger::throwing_logger();
  auto formatter = inja_formatter({});

  SECTION("MarkDown is Supported") {
    const auto node = formatter.build("`code`", {});
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
    const auto node = formatter.build(R"({{ md_escape("`code`") }})", {});
    REQUIRE(output_generator::xml::xml_generator::render(node) == util::unindent(R"(
      <?xml version="1.0"?>
      <document>
        <paragraph>`code`</paragraph>
      </document>
      )"));
  }
}

}
