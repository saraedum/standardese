// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <cppast/cpp_type.hpp>
#include <cppast/cpp_template.hpp>
#include <cppast/forward.hpp>
#include <fmt/format.h>

#include "inja_formatter.impl.hpp"
#include "../../standardese/logger.hpp"

namespace standardese::formatter {

nlohmann::json inja_formatter::arguments_callback(const nlohmann::json& data) const {
  return std::visit([&](auto&& entity) {
    using T = std::decay_t<decltype(entity)>;
    if constexpr (std::is_same_v<T, const cppast::cpp_type*>) {
      return std::visit([&](auto&& arguments) -> nlohmann::json {
        using S = std::decay_t<decltype(arguments)>;
        if constexpr (std::is_same_v<S, std::string>) {
          return arguments;
        } else {
          auto ret = nlohmann::json::array();
          for (auto& arg : arguments) {
            std::visit([&](auto& arg) {
              using A = std::decay_t<decltype(arg)>;
              if constexpr (std::is_same_v<A, std::string>) {
                ret.push_back(arg);
              } else {
                ret.push_back(to_json(*arg));
              }
            }, arg);
          }
          return ret;
        }
      }, arguments(*entity));
    }

    logger::error(fmt::format("Template callback `arguments` not valid here. Cannot determine template arguments for {}.", nlohmann::to_string(data)));
    return nlohmann::json::array();
  }, self->from_json(data));
}

std::variant<std::vector<std::variant<const cppast::cpp_type*, std::string>>, std::string> inja_formatter::arguments(const cppast::cpp_type& type) const {
  switch(type.kind()) {
    case cppast::cpp_type_kind::template_instantiation_t:
    {
      const auto& template_instantiation_type = static_cast<const cppast::cpp_template_instantiation_type&>(type);
      if (template_instantiation_type.arguments_exposed()) {
        std::vector<std::variant<const cppast::cpp_type*, std::string>> arguments;
        for (const auto& argument : template_instantiation_type.arguments().value()) {
          if (argument.template_ref().has_value()) {
            arguments.push_back(argument.template_ref().value().name());
          } else if (argument.expression().has_value()) {
            switch(argument.expression().value().kind()) {
              case cppast::cpp_expression_kind::literal_t:
                arguments.push_back(static_cast<const cppast::cpp_literal_expression&>(argument.expression().value()).value());
                break;
              case cppast::cpp_expression_kind::unexposed_t:
                arguments.push_back(static_cast<const cppast::cpp_unexposed_expression&>(argument.expression().value()).expression().as_string());
                break;
              default:
                throw std::logic_error("Unsupported template argument kind.");
            }
          } else {
            arguments.push_back(&argument.type().value());
          }
        }
        return arguments;
      } else {
        return template_instantiation_type.unexposed_arguments();
      }
    }
    default:
      logger::error("Template callback `arguments` not valid here. Type has no template arguments.");
      return std::string{};
  }
}

}

