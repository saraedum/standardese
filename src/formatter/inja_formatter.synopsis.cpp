// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <fmt/format.h>

#include "inja_formatter.impl.hpp"
#include "../../standardese/logger.hpp"
#include "../../standardese/model/document.hpp"

namespace standardese::formatter {

nlohmann::json inja_formatter::synopsis_callback(const nlohmann::json& data) const {
  return std::visit([&](auto&& entity) {
    using T = std::decay_t<decltype(entity)>;
    if constexpr (std::is_same_v<T, const cppast::cpp_entity*>) {
      return to_json(synopsis(*entity));
    }

    logger::error(fmt::format("Template callback `synopsis` not valid here. Cannot produce synopsis of {}.", nlohmann::to_string(data)));
    return to_json(model::document{"", ""});
  }, self->from_json(data));
}

model::document inja_formatter::synopsis(const cppast::cpp_entity& entity) const {
  // TODO: Use \synopsis.
  return code(entity);
}

}
