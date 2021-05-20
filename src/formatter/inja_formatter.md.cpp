// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <fmt/format.h>

#include "inja_formatter.impl.hpp"
#include "../../standardese/logger.hpp"

namespace standardese::formatter {

std::string inja_formatter::md_callback(const nlohmann::json& data) const {
  const auto md = data.find("md");
  if (md != data.end() && md->is_string()) {
    return md->get<std::string>();
  }

  logger::error(fmt::format("Cannot render `{}` as MarkDown in inja callback `md`.", nlohmann::to_string(data)));
  return std::string{};
}

}
