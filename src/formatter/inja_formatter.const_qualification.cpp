// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <fmt/format.h>
#include <cppast/cpp_entity_kind.hpp>
#include <cppast/cpp_type.hpp>
#include <cppast/cpp_member_function.hpp>

#include "inja_formatter.impl.hpp"
#include "../../standardese/logger.hpp"

namespace standardese::formatter {

namespace {

std::string to_string(cppast::cpp_cv cv) {
  switch(cv) {
    case cppast::cpp_cv_const:
    case cppast::cpp_cv_const_volatile:
      return "const";
    default:
      return "";
  }
}

}

std::string inja_formatter::const_qualification_callback(const nlohmann::json& data) const {
  return std::visit([&](auto&& entity) {
    using T = std::decay_t<decltype(entity)>;
    if constexpr (std::is_same_v<T, const cppast::cpp_entity*>) {
      return const_qualification(*entity);
    } else if constexpr (std::is_same_v<T, const cppast::cpp_type*>) {
      return const_qualification(*entity);
    }

    logger::error(fmt::format("Template callback `const_qualification` not valid here. Cannot determine const qualification of {}.", nlohmann::to_string(data)));
    return std::string{};
  }, self->from_json(data));
}

std::string inja_formatter::const_qualification(const cppast::cpp_entity& entity) const {
  switch(entity.kind()) {
    case cppast::cpp_entity_kind::member_function_t:
    case cppast::cpp_entity_kind::conversion_op_t:
      return to_string(static_cast<const cppast::cpp_member_function_base&>(entity).cv_qualifier());
    default:
      // No other entity can be "const". All the other "const" keywords refer
      // to types. (Or are we missing something here?)
      return std::string{};
  }
}

std::string inja_formatter::const_qualification(const cppast::cpp_type& type) const {
  switch(type.kind()) {
    case cppast::cpp_type_kind::cv_qualified_t:
      return to_string(static_cast<const cppast::cpp_cv_qualified_type&>(type).cv_qualifier());
    default:
      return std::string{};
}

}

}
