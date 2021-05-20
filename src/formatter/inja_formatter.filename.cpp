// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <boost/filesystem/path.hpp>
#include <fmt/format.h>

#include "inja_formatter.impl.hpp"
#include "../../standardese/logger.hpp"

namespace standardese::formatter {

std::string inja_formatter::filename_callback(const nlohmann::json& data) const {
    return std::visit([&](auto&& entity) {
      using T = std::decay_t<decltype(entity)>;
      if constexpr (std::is_same_v<T, const nlohmann::json::string_t*>) {
        return filename(*entity);
      }

      logger::error(fmt::format("Cannot determine filename of `{}` in inja callback `filename`.", nlohmann::to_string(data)));
      return std::string{};
    }, self->from_json(data));
}

std::string inja_formatter::filename(const std::string& path) const {
  return boost::filesystem::path(path).filename().string();
}

}
