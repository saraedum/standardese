// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <fmt/format.h>
#include <cppast/cpp_entity.hpp>

#include "inja_formatter.impl.hpp"
#include "../../standardese/logger.hpp"

namespace standardese::formatter {

nlohmann::json inja_formatter::synopsis_callback(const nlohmann::json& data) const {
  return std::visit([&](auto&& entity) {
    using T = std::decay_t<decltype(entity)>;
    if constexpr (std::is_same_v<T, const cppast::cpp_entity*>) {
      return synopsis(*entity);
    }

    logger::error(fmt::format("Template callback `synopsis` not valid here. Cannot request synopsis of {}.", nlohmann::to_string(data)));
    return nlohmann::json{};
  }, self->from_json(data));
}

nlohmann::json inja_formatter::synopsis(const cppast::cpp_entity& entity) const {
  // TODO: Use \synopsis.
  return nlohmann::json{};
}

}
