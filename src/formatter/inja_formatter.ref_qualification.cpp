// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <cppast/cpp_type.hpp>
#include <fmt/format.h>

#include "inja_formatter.impl.hpp"
#include "../../standardese/logger.hpp"

namespace standardese::formatter {

std::string inja_formatter::ref_qualification_callback(const nlohmann::json& data) const {
  return std::visit([&](auto&& entity) {
    using T = std::decay_t<decltype(entity)>;
    if constexpr (std::is_same_v<T, const cppast::cpp_entity*>) {
      return ref_qualification(*entity);
    } else if constexpr (std::is_same_v<T, const cppast::cpp_type*>) {
      return ref_qualification(*entity);
    }

    logger::error(fmt::format("Template callback `ref_qualification` not valid here. Cannot determine ref qualification of {}.", nlohmann::to_string(data)));
    return std::string{};
  }, self->from_json(data));
}

std::string inja_formatter::ref_qualification(const cppast::cpp_entity& entity) const {
  // TODO
  return "";
}

std::string inja_formatter::ref_qualification(const cppast::cpp_type& type) const {
  switch (type.kind()) {
    case cppast::cpp_type_kind::reference_t:
      switch (static_cast<const cppast::cpp_reference_type&>(type).reference_kind()) {
        case cppast::cpp_ref_lvalue:
          return "&";
        case cppast::cpp_ref_rvalue:
          return "&&";
        default:
          return "";
      }
    default:
      break;
  }
  return "";
}

}
