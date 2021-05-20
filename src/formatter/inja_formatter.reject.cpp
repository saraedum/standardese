// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <algorithm>
#include <fmt/format.h>

#include "inja_formatter.impl.hpp"

#include "../../standardese/logger.hpp"

namespace standardese::formatter {

nlohmann::json inja_formatter::reject_callback(const nlohmann::json& predicate, const nlohmann::json& items) const {
  return std::visit([&](auto&& p, auto&& i) {
    using P = std::decay_t<decltype(p)>;
    using L = std::decay_t<decltype(i)>;
    if constexpr (std::is_same_v<P, const nlohmann::json::string_t*>) {
      if constexpr (std::is_same_v<L, const nlohmann::json::array_t*>) {
        return reject(*p, *i);
      } else {
        logger::error(fmt::format("Template callback `reject` not valid here. Cannot use {} as a list of items.", nlohmann::to_string(items)));
      }
    } else {
      logger::error(fmt::format("Template callback `reject` not valid here. Cannot use {} as a predicate.", nlohmann::to_string(predicate)));
    }
    return std::vector<nlohmann::json>{};
  }, self->from_json(predicate), self->from_json(items));
}

std::vector<nlohmann::json> inja_formatter::reject(const std::string& predicate, std::vector<nlohmann::json> items) const {
  std::function<bool(const nlohmann::json&)> pred = [](auto&&) { return false; };

  if (predicate == "empty") {
    pred = [&](const nlohmann::json& item) {
      return std::visit([&](auto&& i) {
        using T = std::decay_t<decltype(i)>;
        if constexpr (std::is_same_v<T, const nlohmann::json::string_t*>) {
          return i->empty();
        }
        logger::error(fmt::format("Cannot apply callback `reject` with predicate `empty` to {}", item));
        return true;
      }, this->self->from_json(item));
    };
  } else if (predicate == "whitespace") {
    pred = [&](const nlohmann::json& item) {
      return std::visit([&](auto&& i) {
        using T = std::decay_t<decltype(i)>;
        if constexpr (std::is_same_v<T, const nlohmann::json::string_t*>) {
          return std::all_of(begin(*i), end(*i), ::isspace);
        }
        logger::error(fmt::format("Cannot apply callback `reject` with predicate `whitespace` to {}", item));
        return true;
      }, this->self->from_json(item));
    };
  } else {
    logger::error(fmt::format("Unknown predicate `{}` in invocation of inja template `reject()`.", predicate));
  }

  std::vector<nlohmann::json> filtered;
  
  for (auto& item: items)
    if (!pred(item)) {
      filtered.push_back(std::move(item));
    } else {
      logger::trace([&]() { return fmt::format("Rejecting {} as it is `{}`.", nlohmann::to_string(item), predicate); });
    }

  return filtered;
}

}
