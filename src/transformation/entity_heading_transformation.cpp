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

#include "../../standardese/transformation/entity_heading_transformation.hpp"
#include "../../standardese/model/visitor/visit.hpp"
#include "../../standardese/model/entity.hpp"
#include "../../standardese/model/cpp_entity_documentation.hpp"
#include "../../standardese/model/markup/heading.hpp"
#include "../../standardese/model/markup/code.hpp"
#include "../../standardese/model/document.hpp"
#include "../../standardese/model/markup/paragraph.hpp"
#include "../../standardese/output_generator/xml/xml_generator.hpp"
#include "../../standardese/parser/comment_parser.hpp"
#include "../../standardese/parser/cpp_context.hpp"
#include "../../standardese/logger.hpp"
#include "../../standardese/parser/cppast_parser.hpp"
#include "../../standardese/formatter/inja_formatter.hpp"

namespace standardese::transformation {

namespace {

template <typename T>
model::document heading(T& documentation, parser::cpp_context, const entity_heading_transformation::entity_heading_transformation_options&);

}

entity_heading_transformation::entity_heading_transformation(model::unordered_entities& entities, parser::cpp_context cpp_context, struct entity_heading_transformation_options options) : transformation(entities), options(options), cpp_context(std::move(cpp_context)) {}

entity_heading_transformation::entity_heading_transformation_options::entity_heading_transformation_options(formatter::inja_formatter::inja_formatter_options inja_formatter_options) : inja_formatter_options(std::move(inja_formatter_options)) {}

void entity_heading_transformation::do_transform(model::entity& document) {
  int level = 0;

  model::visitor::visit([&](auto&& entity, auto&& recurse) {
    using T = std::decay_t<decltype(entity)>;

    if constexpr (std::is_base_of_v<model::mixin::documentation, T>) {
      auto doc = heading(entity, cpp_context, options);
      bool has_scope = doc.begin() != doc.end() && doc.begin()->template is<model::markup::heading>();
      for (auto paragraph = doc.rbegin(); paragraph != doc.rend(); ++paragraph) {
        if (paragraph->template is<model::markup::heading>())
          // TODO: Cap at 5?
          paragraph->template as<model::markup::heading>().level += level;

        entity.insert_child(std::move(*paragraph)); 
      }

      if (has_scope) level++;

      recurse();

      if (has_scope) level--;
    } else {
      recurse();
    }
  }, document);
}

namespace {

template <typename T>
model::document heading(T& documentation, parser::cpp_context context, const entity_heading_transformation::entity_heading_transformation_options& options) {
  formatter::inja_formatter inja{options.inja_formatter_options, context, documentation};
 inja.data().merge_patch(inja.to_json(documentation));

  // TODO:
  // logger::debug([&]() { return fmt::format("Generating heading for {} `{}`.", cppast::to_string(documentation.entity().kind()), inja.name(documentation.entity())); });

  // TODO: Move implementation out.
  inja.add_callback("section", [&](std::vector<const nlohmann::json*> args) {
    if (!args.at(0)->is_string()) {
      logger::error(fmt::format("Template callback `section()` expects a string argument but found {}.", nlohmann::to_string(*args.at(0))));
      return nlohmann::json{};
    }

    if (args.size() != 1) {
      logger::warn(fmt::format("Ignoring trailing argument {}. Template callback `section()` expected exactly one string argument.", nlohmann::to_string(*args.at(1))));
    }

    // TODO: Actually parse argument.

    auto brief = documentation.section(parser::commands::section_command::brief);
    if (brief)
      return inja.to_json(brief.value());

    return inja.to_json(model::section(parser::commands::section_command::brief));
  });

  // TODO: Move implementation out.
  inja.add_void_callback("drop_section", [&](std::vector<const nlohmann::json*> args) {
    if (!args.at(0)->is_string()) {
      logger::error(fmt::format("Template callback `drop_section()` expects a string argument but found {}.", nlohmann::to_string(*args.at(0))));
      return;
    }

    if (args.size() != 1) {
      logger::warn(fmt::format("Ignoring trailing argument {}. Template callback `drop_section()` expected exactly one string argument.", nlohmann::to_string(*args.at(1))));
    }

    // TODO: Actually parse argument.

    for (auto section = documentation.begin(); section != documentation.end(); ++section) {
      if (section->template is<model::section>() && section->template as<model::section>().type == parser::commands::section_command::brief) {
        documentation.erase(section);
        return;
      }
    }
  });

  // TODO: Use the same format string.
  auto format = type_safe::ref(options.format);
  if constexpr (std::is_same_v<T, model::group_documentation>) {
    format = type_safe::ref(options.group_format);
  }

  return inja.build(*format);
}

}

}
