// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <fmt/format.h>
#include <cppast/cpp_entity_kind.hpp>
#include <cppast/cpp_function.hpp>
#include <cppast/cpp_member_function.hpp>

#include "inja_formatter.impl.hpp"
#include "../../standardese/logger.hpp"

namespace standardese::formatter {

nlohmann::json inja_formatter::return_type_callback(const nlohmann::json& data) const {
  return std::visit([&](auto&& entity) {
    using T = std::decay_t<decltype(entity)>;
    if constexpr (std::is_same_v<T, const cppast::cpp_entity*>) {
      switch(entity->kind()) {
        case cppast::cpp_entity_kind::function_t:
        case cppast::cpp_entity_kind::member_function_t:
          return to_json(return_type(*entity));
      }
    }
    // TODO: Use to_string() in messages everywhere.
    logger::error(fmt::format("Template callback `return_type` not valid here. Cannot determine return type for {}.", self->to_string(data)));
    return nlohmann::json{};
  }, self->from_json(data));
}

const cppast::cpp_type& inja_formatter::return_type(const cppast::cpp_entity& entity) const {
  switch (entity.kind()) {
    case cppast::cpp_entity_kind::function_t:
      return static_cast<const cppast::cpp_function&>(entity).return_type();
    case cppast::cpp_entity_kind::member_function_t:
      return static_cast<const cppast::cpp_member_function&>(entity).return_type();
    default:
      logger::error(fmt::format("Entity {} has no return type. Cannot use template callback `return_type()` in this context.", entity.name()));
      throw std::logic_error("Entity has no return type.");
  }
}

}
