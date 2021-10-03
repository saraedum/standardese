// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <cppast/cpp_file.hpp>
#include <fmt/format.h>
#include <boost/filesystem/path.hpp>
#include <boost/algorithm/string.hpp>

#include "inja_formatter.impl.hpp"
#include "../../standardese/logger.hpp"

namespace standardese::formatter {

std::string inja_formatter::relative_callback(const nlohmann::json& cpp_entity, const nlohmann::json& data) const {
    return std::visit([&](auto&& entity) {
      using T = std::decay_t<decltype(entity)>;

      return std::visit([&](auto&& path_array) {
        using S = std::decay_t<decltype(path_array)>;

        if constexpr (std::is_same_v<T, const cppast::cpp_entity*>) {
          if constexpr (std::is_same_v<S, const nlohmann::json::array_t*>) {
            std::vector<std::string> paths;
            for (auto&& path : *path_array) {
              if (path.is_string())
                paths.push_back(path);
              else
                logger::warn(fmt::format("Ignoring entry {} of which is not a string. Second parameter to `relative` must be an array of strings but found {}.", self->to_string(path), self->to_string(data)));
            }
            return relative(*entity, paths);
          }
        }

        logger::error(fmt::format("Cannot determine path of `{}` relative to `{}` in inja callback `relative`.", self->to_string(cpp_entity), self->to_string(data)));
        return std::string{};
      }, self->from_json(data));
    }, self->from_json(cpp_entity));
}

std::string inja_formatter::relative(const cppast::cpp_entity& entity, const std::vector<std::string>& paths) const {
  const auto absolute = boost::filesystem::path{this->absolute(entity)};

  if (paths.size() == 0)
    return absolute.string();

  boost::filesystem::path gcd = absolute.parent_path();

  for (auto&& path : paths) {
    auto normal = boost::filesystem::path{path}.lexically_normal();
    while (boost::algorithm::starts_with(normal.lexically_relative(gcd).string(), "..")) {
      if (gcd == gcd.root_path())
        break;
      gcd = gcd.parent_path();
    }
  }

  if (gcd == gcd.root_path())
    return absolute.string();

  const auto relative = absolute.lexically_relative(gcd).string();
  if (relative == "." || relative == "")
    return absolute.filename().string();
  return relative;
}

}
