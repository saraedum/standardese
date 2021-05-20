// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_TRANSFORMATION_ENTITY_HEADING_TRANSFORMATION_HPP_INCLUDED
#define STANDARDESE_TRANSFORMATION_ENTITY_HEADING_TRANSFORMATION_HPP_INCLUDED

#include "transformation.hpp"
#include "../formatter/inja_formatter.hpp"

namespace standardese::transformation {

/// Inserts headings into a document, e.g., for all
/// [model::cpp_entity_documentation]() entities.
class entity_heading_transformation : public transformation {
  public:
    struct entity_heading_transformation_options {
      entity_heading_transformation_options(formatter::inja_formatter::inja_formatter_options = {});

      // TODO: Read from CLI and reset the default to standardese 0-5-0 equivalent.
      std::string format = R"({% if cppast_kind == "file" %}# {{ join(" — ", reject("whitespace", list(md_escape(name), md(section("brief"))))) }}{{ drop_section("brief") }}
        {%- else if cppast_kind in ["function", "member function", "conversion op", "constructor", "destructor", "function template", "friend"] %}# {{ md(synopsis) }}
        {%- else if cppast_kind == "function_parameter" %}###### `{{ code_escape(name) }}` {{ md(section("brief")) }}{{ drop_section("brief") }}
        {%- else %}# {{ md_escape(kind) }} `{{ code_escape(name) }}`
        {%- endif %})";

      std::string group_format = R"(# {{ standardese.output_section }}
```
{% for member in standardese.entities %}({{ loop.index1 }}) {{ text(synopsis(member)) }}
{% endfor %}```)";

      formatter::inja_formatter::inja_formatter_options inja_formatter_options;
    };

    entity_heading_transformation(model::unordered_entities& entities, entity_heading_transformation_options = {});

  protected:
    void do_transform(model::entity& root) override;

  private:
    struct entity_heading_transformation_options options;
};

}

#endif
