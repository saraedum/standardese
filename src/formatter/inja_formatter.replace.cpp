// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <fmt/format.h>
#include <regex>

#include "inja_formatter.impl.hpp"
#include "../../standardese/logger.hpp"

namespace standardese::formatter {

std::string inja_formatter::replace_callback(const nlohmann::json& str_, const nlohmann::json& pattern_, const nlohmann::json& replacement_) const {
  return std::visit([&](auto&& str, auto&& pattern, auto&& replacement) {
    using T = std::decay_t<decltype(str)>;
    if constexpr (!std::is_same_v<T, const nlohmann::json::string_t*>) {
      logger::error(fmt::format("Template callback `replace` expected a string but found.", nlohmann::to_string(str_)));
      return std::string{};
    } else {
      using T = std::decay_t<decltype(pattern)>;
      if constexpr (!std::is_same_v<T, const nlohmann::json::string_t*>) {
        logger::error(fmt::format("Template callback `replace` expected a string but found.", nlohmann::to_string(pattern_)));
        return std::string{};
      } else {
        using T = std::decay_t<decltype(replacement)>;
        if constexpr (!std::is_same_v<T, const nlohmann::json::string_t*>) {
          logger::error(fmt::format("Template callback `replace` expected a string but found.", nlohmann::to_string(replacement_)));
          return std::string{};
        } else {
          return replace(*str, *pattern, *replacement);
        }
      }
    }
  }, self->from_json(str_), self->from_json(pattern_), self->from_json(replacement_));
}

std::string inja_formatter::replace(const std::string& str, const std::string& pattern, const std::string& replacement) const {
  return std::regex_replace(str, std::regex(pattern), replacement);
}

}


