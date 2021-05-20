// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <fmt/format.h>

#include "inja_formatter.impl.hpp"
#include "../../standardese/logger.hpp"

namespace standardese::formatter {

std::string inja_formatter::join_callback(const nlohmann::json& separator, const nlohmann::json& entities) const {
  return std::visit([&](auto&& sep, auto&& items) {
    using S = std::decay_t<decltype(sep)>;
    using T = std::decay_t<decltype(items)>;
    if constexpr (std::is_same_v<S, const nlohmann::json::string_t*>) {
      if constexpr (std::is_same_v<T, const nlohmann::json::array_t*>) {
        std::vector<std::string> strings;

        for (const auto& string : *items) {
          std::visit([&](auto&& str) {
            using STR = std::decay_t<decltype(str)>;
            if constexpr (std::is_same_v<STR, const nlohmann::json::string_t*>) {
              strings.push_back(*str);
            } else {
              logger::error(fmt::format("Cannot join {} with template callback `join`.", nlohmann::json(string)));
            }
          }, self->from_json(string));
        }

        return join(*sep, std::move(strings));
      } else {
        logger::error(fmt::format("Template callback `join` cannot be used with non-array {}.", nlohmann::to_string(entities)));
      }
    } else {
      logger::error(fmt::format("Template callback `join` cannot be used with separator {}.", nlohmann::to_string(separator)));
    }

    return std::string{};
  }, self->from_json(separator), self->from_json(entities));
}

std::string inja_formatter::join(const std::string& separator, std::vector<std::string> items) const {
  std::stringstream joined;
  for (auto it = items.begin(); it != items.end(); ++it) {
    if (it != items.begin())
      joined << separator;
    joined << std::move(*it);
  }

  return joined.str();
}

}
