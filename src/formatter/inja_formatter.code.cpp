// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <fmt/format.h>

#include "inja_formatter.impl.hpp"
#include "../../standardese/logger.hpp"
#include "../../standardese/model/document.hpp"
#include "../../standardese/model/visitor/visit.hpp"
#include "../../standardese/output_generator/xml/xml_generator.hpp"

namespace standardese::formatter {

namespace {

// Collapse code only separated by spaces, i.e., turn `a` `b` into `ab`.
model::document simplify_code(model::document&& root) {
  logger::trace([&]() { return fmt::format("Simplifying code in {}.", output_generator::xml::xml_generator::render(root)); });

  model::visitor::visit([&](auto& entity, auto&& recurse) {
    using T = std::decay_t<decltype(entity)>;

    recurse();

    if constexpr (std::is_base_of_v<model::mixin::container<>, T>) {
      std::vector<model::entity> children;

      for (model::entity& child : entity) {
        if (children.size() >= 2 && child.is<model::markup::code>()) {
          auto& merge_from = child.as<model::markup::code>();
          auto& whitespace = *children.rbegin();
          if (whitespace.is<model::markup::text>() && whitespace.as<model::markup::text>().value.find_first_not_of(' ') == std::string::npos) {
            
            auto& code = *(++children.rbegin());
            if (code.is<model::markup::code>()) {
              auto& merge_into = code.as<model::markup::code>();
              if (merge_from.begin() == merge_from.end()) {
                // Handle whitespace in Markdown code i.e. ` ` which is
                // encoded by an empty model::markup::code.
                merge_into.add_child(model::markup::text(" "));
              } else if (++merge_from.begin() == merge_from.end() && merge_from.begin()->is<model::markup::text>() && merge_from.begin()->as<model::markup::text>().value == "") {
                // Handle whitespace in Markdown code which was explicitly
                // encoded as "".
                merge_into.add_child(model::markup::text(" "));
              } else {
                for (model::entity& c : merge_from) {
                  merge_into.add_child(std::move(c));
                }
              }

              // Drop the whitespace text.
              children.pop_back();

              // Ignore this child since we merged it with the preceding code.
              continue;
            }
          }
        }

        children.push_back(std::move(child));
      }

      entity.clear();

      for (model::entity& child : children)
        entity.add_child(std::move(child));
    }
  }, root);

  logger::trace([&]() { return fmt::format("Simplified code to {}.", output_generator::xml::xml_generator::render(root)); });

  return std::move(root);
}

}

nlohmann::json inja_formatter::code_callback(const nlohmann::json& data) const {
  return std::visit([&](auto&& entity) {
    using T = std::decay_t<decltype(entity)>;
    if constexpr (std::is_same_v<T, const cppast::cpp_entity*>) {
      return to_json(code(*entity));
    } else if constexpr (std::is_same_v<T, const nlohmann::json::string_t*>) {
      return code_callback(data, this->data());
    }

    logger::error(fmt::format("Template callback `code` not valid here. Cannot produce code for {}.", nlohmann::to_string(data)));
    return to_json(model::document{"", ""});
  }, self->from_json(data));
}

nlohmann::json inja_formatter::code_callback(const nlohmann::json& format, const nlohmann::json& entity) const {
  return std::visit([&](auto&& format_string, auto&& cpp_entity) {
    using T = std::decay_t<decltype(format_string)>;
    using S = std::decay_t<decltype(cpp_entity)>;
    if constexpr (std::is_same_v<T, const nlohmann::json::string_t*>) {
      if constexpr (std::is_same_v<S, const cppast::cpp_entity&>) {
        return to_json(code(*format_string, *cpp_entity));
      } else {
        logger::error(fmt::format("Template callback `code` not valid here. Cannot produce code for {}.", nlohmann::to_string(entity)));
      }
    } else {
      logger::error(fmt::format("Template callback `code` not valid here. Argument {} is not a format string.", nlohmann::to_string(format)));
    }
    return to_json(model::document{"", ""});
  }, self->from_json(format), self->from_json(entity));
}

model::document inja_formatter::code(const cppast::cpp_entity& entity) const {
  switch(entity.kind()) {
    case cppast::cpp_entity_kind::function_t:
    case cppast::cpp_entity_kind::member_function_t:
    case cppast::cpp_entity_kind::constructor_t:
    case cppast::cpp_entity_kind::destructor_t:
    case cppast::cpp_entity_kind::conversion_op_t:
      return code(self->options.function_format, entity);
    case cppast::cpp_entity_kind::friend_t:
      return code(self->options.friend_format, entity);
    default:
      // TODO
      logger::error(fmt::format("not implemented: code() for `{}`.", name(entity)));
      return model::document{"", ""};
  }
}

model::document inja_formatter::code(const std::string& format, const cppast::cpp_entity& entity) const {
  auto finally = impl::savepoint(const_cast<inja_formatter::impl&>(*this->self));
  const_cast<inja_formatter*>(this)->data() = to_json(entity);
  return simplify_code(build(format));
}

}
