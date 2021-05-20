// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <regex>
#include <fmt/format.h>

#include "inja_formatter.impl.hpp"
#include "../../standardese/logger.hpp"

namespace standardese::formatter {

std::string inja_formatter::sanitize_basename_callback(const nlohmann::json& data) const {
  return std::visit([&](auto&& entity) {
    using T = std::decay_t<decltype(entity)>;
    if constexpr (std::is_same_v<T, const nlohmann::json::string_t*>) {
      return sanitize_basename(*entity);
    }

    logger::error(fmt::format("Template callback `sanitize_basename` not valid here. Cannot sanitize name of {}.", nlohmann::to_string(data)));
    return std::string{};
  }, self->from_json(data));
}

std::string inja_formatter::sanitize_basename(const std::string& basename) const {
  static std::regex forbidden{R"((\W|_)+)"};
  return std::regex_replace(basename, forbidden, "_");
}

}
