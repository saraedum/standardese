// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <cppast/cpp_type.hpp>
#include <cppast/cpp_entity_kind.hpp>

#include <fmt/format.h>

#include "inja_formatter.impl.hpp"
#include "../../standardese/logger.hpp"

namespace standardese::formatter {

std::string inja_formatter::cppast_kind_callback(const nlohmann::json& data) const {
  return std::visit([&](auto&& entity) {
    using T = std::decay_t<decltype(entity)>;
    if constexpr (std::is_same_v<T, const cppast::cpp_entity*>) {
      return cppast_kind(*entity);
    } else if constexpr (std::is_same_v<T, const cppast::cpp_type*>) {
      return cppast_kind(*entity);
    }

    logger::error(fmt::format("Template callback `cppast_kind` not valid here. Cannot determine kind of {}.", nlohmann::to_string(data)));
    return std::string{};
  }, self->from_json(data));
}

std::string inja_formatter::cppast_kind(const cppast::cpp_entity& entity) const {
  return cppast::to_string(entity.kind());
}

std::string inja_formatter::cppast_kind(const cppast::cpp_type& type) const {
  switch (type.kind()) {
    case cppast::cpp_type_kind::builtin_t:
      return "builtin";
    case cppast::cpp_type_kind::user_defined_t:
      return "user-defined";
    case cppast::cpp_type_kind::auto_t:
      return "auto";
    case cppast::cpp_type_kind::decltype_t:
      return "decltype";
    case cppast::cpp_type_kind::decltype_auto_t:
      return "decltype(auto)";
    case cppast::cpp_type_kind::cv_qualified_t:
      return "cv-qualified";
    case cppast::cpp_type_kind::pointer_t:
      return "pointer";
    case cppast::cpp_type_kind::reference_t:
      return "reference";
    case cppast::cpp_type_kind::array_t:
      return "array";
    case cppast::cpp_type_kind::function_t:
      return "function";
    case cppast::cpp_type_kind::member_function_t:
      return "member function";
    case cppast::cpp_type_kind::member_object_t:
      return "member object";
    case cppast::cpp_type_kind::template_parameter_t:
      return "template parameter";
    case cppast::cpp_type_kind::template_instantiation_t:
      return "template instantiation";
    case cppast::cpp_type_kind::dependent_t:
      return "dependent";
    case cppast::cpp_type_kind::unexposed_t:
      return "unexposed";
  }
}

}

