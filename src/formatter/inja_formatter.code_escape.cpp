// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <fmt/format.h>

#include "inja_formatter.impl.hpp"
#include "../../standardese/logger.hpp"
#include "../../standardese/parser/markdown_parser.hpp"

namespace standardese::formatter {

std::string inja_formatter::code_escape_callback(const nlohmann::json& data) const {
  return std::visit([&](auto&& entity) {
    using T = std::decay_t<decltype(entity)>;
    if constexpr (std::is_same_v<T, const nlohmann::json::string_t*>) {
      return code_escape(*entity);
    }

    logger::error(fmt::format("Template callback `code_escape` not valid here. Cannot escape {} for a code black.", nlohmann::to_string(data)));
    return std::string{};
  }, self->from_json(data));
}

std::string inja_formatter::code_escape(const std::string& code) const {
  // TODO
  return code;
}

}

