// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <cppast/cpp_file.hpp>
#include <fmt/format.h>

#include "inja_formatter.impl.hpp"
#include "../../standardese/logger.hpp"

namespace standardese::formatter {

std::string inja_formatter::path_callback(const nlohmann::json& data) const {
    return std::visit([&](auto&& entity) {
      using T = std::decay_t<decltype(entity)>;
      if constexpr (std::is_same_v<T, const cppast::cpp_entity*>) {
        return path(*entity);
      }
      logger::error(fmt::format("Cannot determine path of `{}` in inja callback `path`.", nlohmann::to_string(data)));
      return std::string{};
    }, self->from_json(data));
}

std::string inja_formatter::path(const cppast::cpp_entity& entity) const {
  const auto* parent = &entity;
  while(parent->parent().has_value())
    parent = &parent->parent().value();
  return static_cast<const cppast::cpp_file&>(*parent).name();
}

}

