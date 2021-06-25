// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <fmt/format.h>
#include <cppast/cpp_entity_kind.hpp>
#include <cppast/cpp_entity.hpp>
#include <cppast/cpp_variable.hpp>
#include <cppast/cpp_member_variable.hpp>

#include "inja_formatter.impl.hpp"
#include "../../standardese/logger.hpp"

namespace standardese::formatter {

nlohmann::json inja_formatter::variable_type_callback(const nlohmann::json& data) const {
  return std::visit([&](auto&& entity) {
    using T = std::decay_t<decltype(entity)>;
    if constexpr (std::is_same_v<T, const cppast::cpp_entity*>) {
      switch(entity->kind()) {
        case cppast::cpp_entity_kind::variable_t:
        case cppast::cpp_entity_kind::member_variable_t:
          return to_json(variable_type(*entity));
      }
    }
    // TODO: Use to_string() in messages everywhere.
    logger::error(fmt::format("Template callback variable_type` not valid here. Cannot determine variable type for {}.", self->to_string(data)));
    return nlohmann::json{};
  }, self->from_json(data));
}

const cppast::cpp_type& inja_formatter::variable_type(const cppast::cpp_entity& entity) const {
  switch (entity.kind()) {
    case cppast::cpp_entity_kind::variable_t:
      return static_cast<const cppast::cpp_variable&>(entity).type();
    case cppast::cpp_entity_kind::member_variable_t:
      return static_cast<const cppast::cpp_member_variable_base&>(entity).type();
    default:
      logger::error(fmt::format("Entity {} has no return type. Cannot use template callback `variable_type()` in this context.", entity.name()));
      throw std::logic_error("Entity has no variable type.");
  }
}

}

