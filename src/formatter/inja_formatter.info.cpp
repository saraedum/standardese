// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include "inja_formatter.impl.hpp"

#include "../../standardese/logger.hpp"

namespace standardese::formatter {

void inja_formatter::info_callback(const nlohmann::json& data) const {
  std::visit([&](auto&& entity) {
    using T = std::decay_t<decltype(entity)>;
    if constexpr (std::is_same_v<T, const nlohmann::json::string_t*>) {
      info(*entity);
    } else {
      info(nlohmann::to_string(data));
    }
  }, self->from_json(data));
}

void inja_formatter::info(const std::string& message) const {
  logger::info(message);
}

}
