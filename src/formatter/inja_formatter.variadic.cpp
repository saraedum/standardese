// Copyright (C) 2022 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <cppast/cpp_template_parameter.hpp>
#include <cppast/cpp_entity_kind.hpp>
#include <fmt/format.h>

#include "inja_formatter.impl.hpp"
#include "../../standardese/logger.hpp"

namespace standardese::formatter {

nlohmann::json inja_formatter::variadic_callback(const nlohmann::json& data) const {
  return std::visit([&](auto&& entity) {
    using T = std::decay_t<decltype(entity)>;
    if constexpr (std::is_same_v<T, const cppast::cpp_entity*>) {
      switch(entity->kind()) {
      case cppast::cpp_entity_kind::template_type_parameter_t:
      case cppast::cpp_entity_kind::non_type_template_parameter_t:
      case cppast::cpp_entity_kind::template_template_parameter_t:
        return nlohmann::json(variadic(*static_cast<const cppast::cpp_template_parameter*>(entity)));
      default:
        break;
      }
    }

    logger::error(fmt::format("Template callback `variadic` not valid here. Cannot determine whether {} is variadic.", self->to_string(data)));
    return nlohmann::json{false};
  }, self->from_json(data));
}

bool inja_formatter::variadic(const cppast::cpp_template_parameter& parameter) const {
  return parameter.is_variadic();
}

}
