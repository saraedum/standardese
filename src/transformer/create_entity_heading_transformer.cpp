// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <boost/filesystem.hpp>
#include <cppast/cpp_file.hpp>
#include <cppast/cpp_class.hpp>
#include <cppast/cpp_template.hpp>
#include <cppast/cpp_type_alias.hpp>
#include <type_safe/optional.hpp>
#include <fmt/format.h>
#include <nlohmann/json.hpp>
#include <type_traits>

#include "../../standardese/transformer/create_entity_heading_transformer.hpp"
#include "../../standardese/model/visitor/visit.hpp"
#include "../../standardese/model/entity.hpp"
#include "../../standardese/model/cpp_entity_documentation.hpp"
#include "../../standardese/model/markup/heading.hpp"
#include "../../standardese/model/markup/code.hpp"
#include "../../standardese/model/document.hpp"
#include "../../standardese/model/markup/paragraph.hpp"
#include "../../standardese/output_generator/xml/xml_generator.hpp"
#include "../../standardese/parser/cpp_context.hpp"
#include "../../standardese/logger.hpp"
#include "../../standardese/parser/cppast_parser.hpp"
#include "../../standardese/formatter/inja_formatter.hpp"

namespace standardese::transformer {

namespace {

template <typename T>
model::document heading(T& documentation, parser::cpp_context, type_safe::optional_ref<const cppast::cpp_entity>, const create_entity_heading_transformer::create_entity_heading_transformer_options&);

}

create_entity_heading_transformer::create_entity_heading_transformer(model::unordered_entities& entities, parser::cpp_context cpp_context, struct create_entity_heading_transformer_options options) : inner_transformer(entities), options(options), cpp_context(std::move(cpp_context)) {}

create_entity_heading_transformer::create_entity_heading_transformer_options::create_entity_heading_transformer_options(formatter::inja_formatter::inja_formatter_options inja_formatter_options) : 
  // TODO(0.6.0-alpha): Read from CLI and reset the default to standardese 0-5-0 equivalent.
  format(R"({% if cppast_kind == "file" %}# {{ join(reject("whitespace", list(md_escape(name), md(section("brief")))), " — ") }}{{ drop_section("brief") }}
        {%- else if cppast_kind in ["function", "member function", "conversion operator", "constructor", "destructor", "function template", "friend"] %}# {% if synopsis %}{{ md(code(md_escape(synopsis))) }}{% else %}{{ md(code(md_escape(text(format(option("cpp_format")))))) }}{% endif %}
        {%- else if cppast_kind == "function_parameter" %}###### {{ md(code(md_escape(name))) }} {{ md(section("brief")) }}{{ drop_section("brief") }}
        {%- else %}# {{ md_escape(kind) }} {{ md(code(md_escape(name))) }}
        {%- endif %})"),
  // TODO(0.6.0-alpha): Read from CLI and reset the default to standardese 0-5-0 equivalent.
  group_format(R"(# {{ output_section }}
```
{% for member in entity %}({{ loop.index1 }}) {% if synopsis(entity(member)) %}{{ text(code(md_escape(synopsis(entity(member))))) }}{% else %}{{ text(code(format(option("cpp_format"), entity(member)))) }}{% endif %}{% endfor %}```)"),
  inja_formatter_options(std::move(inja_formatter_options)) {}

void create_entity_heading_transformer::do_transform(model::entity& document) {
  std::vector<type_safe::optional_ref<const cppast::cpp_entity>> level;

  model::visitor::visit([&](auto&& entity, auto&& recurse) {
    using T = std::decay_t<decltype(entity)>;

    if constexpr (std::is_same_v<model::cpp_entity_documentation, T> || std::is_same_v<model::group_documentation, T>) {
      auto doc = heading<T>(entity, cpp_context, level.size() ? level.back() : type_safe::nullopt, options);
      bool has_scope = doc.children.begin() != doc.children.end() && doc.children.begin()->template is<model::markup::heading>();
      for (auto paragraph = doc.children.rbegin(); paragraph != doc.children.rend(); ++paragraph) {
        if (paragraph->template is<model::markup::heading>())
          // TODO(0.6.0-alpha): Cap at 5?
          paragraph->template as<model::markup::heading>().level += level.size();

        entity.children.insert(begin(entity.children), std::move(*paragraph));
      }

      if (has_scope) {
        if constexpr (std::is_same_v<T, model::cpp_entity_documentation>)
          level.push_back(type_safe::ref(entity.entity()));
        else
          level.push_back({});
      }

      recurse();

      if (has_scope)
        level.pop_back();
    } else {
      recurse();
    }
  }, document);
}

namespace {

template <typename T>
model::document heading(T& documentation, parser::cpp_context cpp_context, type_safe::optional_ref<const cppast::cpp_entity> context, const create_entity_heading_transformer::create_entity_heading_transformer_options& options) {
  formatter::inja_formatter inja = [&]() {
    if (context.has_value())
      return formatter::inja_formatter{options.inja_formatter_options, cpp_context, context.value()};
    else
      return formatter::inja_formatter{options.inja_formatter_options, cpp_context};
  }();

  if constexpr (std::is_same_v<model::cpp_entity_documentation, T>) {
    inja.data().merge_patch(inja.to_json(&documentation.entity()));
  } else {
    inja.data().merge_patch(inja.to_json(documentation));
  }

  // TODO(0.6.0-alpha):
  // logger::debug([&]() { return fmt::format("Generating heading for {} `{}`.", cppast::to_string(documentation.entity().kind()), inja.name(documentation.entity())); });

  // TODO(0.6.0-beta): Move implementation out.
  inja.add_callback("section", [&](std::vector<const nlohmann::json*> args) {
    if (!args.at(0)->is_string()) {
      logger::error(fmt::format("Template callback `section()` expects a string argument but found {}.", inja.to_string(*args.at(0))));
      return nlohmann::json{};
    }

    if (args.size() != 1) {
      logger::warn(fmt::format("Ignoring trailing argument {}. Template callback `section()` expected exactly one string argument.", inja.to_string(*args.at(1))));
    }

    // TODO(0.6.0-alpha): Actually parse argument.

    auto brief = documentation.section(parser::commands::section_command::brief);
    if (brief)
      return inja.to_json(brief.value());

    return inja.to_json(model::section(parser::commands::section_command::brief));
  });

  // TODO(0.6.0-beta): Move implementation out.
  inja.add_void_callback("drop_section", [&](std::vector<const nlohmann::json*> args) {
    if (!args.at(0)->is_string()) {
      logger::error(fmt::format("Template callback `drop_section()` expects a string argument but found {}.", inja.to_string(*args.at(0))));
      return;
    }

    if (args.size() != 1) {
      logger::warn(fmt::format("Ignoring trailing argument {}. Template callback `drop_section()` expected exactly one string argument.", inja.to_string(*args.at(1))));
    }

    // TODO(0.6.0-alpha): Actually parse argument.

    for (auto section = documentation.children.begin(); section != documentation.children.end(); ++section) {
      if (section->template is<model::section>() && section->template as<model::section>().type == parser::commands::section_command::brief) {
        documentation.children.erase(section);
        return;
      }
    }
  });

  // TODO(0.6.0-beta): Use the same format string.
  auto format = type_safe::ref(options.format);
  if constexpr (std::is_same_v<T, model::group_documentation>) {
    format = type_safe::ref(options.group_format);
  }

  return inja.parse(inja.format(*format));
}

}

}
