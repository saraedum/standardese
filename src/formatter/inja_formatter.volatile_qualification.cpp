// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <fmt/format.h>
#include <cppast/cpp_entity.hpp>
#include <cppast/cpp_entity_kind.hpp>
#include <cppast/cpp_variable.hpp>
#include <cppast/cpp_member_function.hpp>

#include "inja_formatter.impl.hpp"
#include "../../standardese/logger.hpp"

namespace standardese::formatter {

namespace {

std::string to_string(cppast::cpp_cv cv) {
  switch(cv) {
    case cppast::cpp_cv_volatile:
    case cppast::cpp_cv_const_volatile:
      return "volatile";
    default:
      return "";
  }
}

}

std::string inja_formatter::volatile_qualification_callback(const nlohmann::json& data) const {
  return std::visit([&](auto&& entity) {
    using T = std::decay_t<decltype(entity)>;
    if constexpr (std::is_same_v<T, const cppast::cpp_entity*>) {
      return volatile_qualification(*entity);
    } else if constexpr (std::is_same_v<T, const cppast::cpp_type*>) {
      return volatile_qualification(*entity);
    }

    logger::error(fmt::format("Template callback `volatile_qualification` not valid here. Cannot determine volatile qualification of {}.", nlohmann::to_string(data)));
    return std::string{};
  }, self->from_json(data));
}

std::string inja_formatter::volatile_qualification(const cppast::cpp_entity& entity) const {
  switch(entity.kind()) {
    case cppast::cpp_entity_kind::member_function_t:
    case cppast::cpp_entity_kind::conversion_op_t:
      return to_string(static_cast<const cppast::cpp_member_function_base&>(entity).cv_qualifier());
    default:
      // TODO
      return std::string{};

  }
}

std::string inja_formatter::volatile_qualification(const cppast::cpp_type& type) const {
  switch(type.kind()) {
    case cppast::cpp_type_kind::cv_qualified_t:
      return to_string(static_cast<const cppast::cpp_cv_qualified_type&>(type).cv_qualifier());
    default:
      // TODO:
      return std::string{};
  }
}

}
