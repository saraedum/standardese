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

nlohmann::json inja_formatter::type_callback(const nlohmann::json& data) const {
  return std::visit([&](auto&& entity) {
    using T = std::decay_t<decltype(entity)>;
    if constexpr (std::is_same_v<T, const cppast::cpp_entity*>) {
      switch(entity->kind()) {
        case cppast::cpp_entity_kind::function_parameter_t:
          return to_json(type(*entity));
      }
    } else if constexpr (std::is_same_v<T, const cppast::cpp_type*>) {
      switch(entity->kind()) {
        case cppast::cpp_type_kind::cv_qualified_t:
        case cppast::cpp_type_kind::reference_t:
          return to_json(type(*entity));
      }
    }
    logger::error(fmt::format("Template callback `type` not valid here. Cannot determine type for {}.", nlohmann::to_string(data)));
    return nlohmann::json{};
  }, self->from_json(data));
}

const cppast::cpp_type& inja_formatter::type(const cppast::cpp_entity& entity) const {
  switch (entity.kind()) {
    case cppast::cpp_entity_kind::function_parameter_t:
      return static_cast<const cppast::cpp_function_parameter&>(entity).type();
    default:
      logger::error(fmt::format("Entity {} has no type. Cannot use template callback `type()` in this context.", entity.name()));
      throw std::logic_error("Entity has no type.");
  }
}

const cppast::cpp_type& inja_formatter::type(const cppast::cpp_type& type) const {
  switch (type.kind()) {
    case cppast::cpp_type_kind::cv_qualified_t:
      return static_cast<const cppast::cpp_cv_qualified_type&>(type).type();
    case cppast::cpp_type_kind::reference_t:
      return static_cast<const cppast::cpp_reference_type&>(type).referee();
    default:
      logger::error("Type has no underlying type. Cannot use template callback `type()` in this context.");
      throw std::logic_error("Type has no underlying type.");
  }
}

}

