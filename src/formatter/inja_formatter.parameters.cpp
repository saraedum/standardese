// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <fmt/format.h>
#include <cppast/cpp_function.hpp>
#include <cppast/cpp_entity_kind.hpp>

#include "inja_formatter.impl.hpp"
#include "../../standardese/logger.hpp"
#include "../../standardese/parser/markdown_parser.hpp"

namespace standardese::formatter {

nlohmann::json inja_formatter::parameters_callback(const nlohmann::json& data) const {
  return std::visit([&](auto&& entity) {
    nlohmann::json::array_t params;

    using T = std::decay_t<decltype(entity)>;
    if constexpr (std::is_same_v<T, const cppast::cpp_entity*>) {
      for(auto* param : parameters(*entity))
        params.push_back(to_json(*param));
    } else {
      logger::error(fmt::format("Template callback `parameters` not valid here. Cannot determine parameters for {}.", nlohmann::to_string(data)));
    }
    return params;
  }, self->from_json(data));
}

std::vector<const cppast::cpp_entity*> inja_formatter::parameters(const cppast::cpp_entity& entity) const {
  std::vector<const cppast::cpp_entity*> params;

  switch(entity.kind()) {
    case cppast::cpp_entity_kind::function_t:
    case cppast::cpp_entity_kind::member_function_t:
    case cppast::cpp_entity_kind::constructor_t:
      for (auto& param : static_cast<const cppast::cpp_function_base&>(entity).parameters())
        params.push_back(&param);
      break;
    default:
      logger::error(fmt::format("Entity {} has no parameters. Cannot use template callback `parameters()` in this context.", entity.name()));
      break;
  }

  return params;
}

}
