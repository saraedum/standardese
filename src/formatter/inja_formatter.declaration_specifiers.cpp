// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <fmt/format.h>

#include "inja_formatter.impl.hpp"
#include "../../standardese/logger.hpp"

namespace standardese::formatter {

nlohmann::json inja_formatter::declaration_specifiers_callback(const nlohmann::json& data) const {
  return std::visit([&](auto&& entity) {
    using T = std::decay_t<decltype(entity)>;
    if constexpr (std::is_same_v<T, const cppast::cpp_entity*>) {
      return declaration_specifiers(*entity);
    }

    logger::error(fmt::format("Template callback `declaration_specifiers` not valid here. Cannot determine specifiers of {}.", nlohmann::to_string(data)));
    return std::vector<std::string>{};
  }, self->from_json(data));
}

std::vector<std::string> inja_formatter::declaration_specifiers(const cppast::cpp_entity& entity) const {
  // TODO
  return {};
}

}
