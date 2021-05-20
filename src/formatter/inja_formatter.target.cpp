// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <fmt/format.h>

#include "inja_formatter.impl.hpp"
#include "../../standardese/logger.hpp"
#include "../../standardese/parser/markdown_parser.hpp"

namespace standardese::formatter {

std::string inja_formatter::target_callback(const nlohmann::json& data) const {
  return std::visit([&](auto&& entity) {
    using T = std::decay_t<decltype(entity)>;
    if constexpr (std::is_same_v<T, const cppast::cpp_entity*>) {
      return target(*entity);
    } else if constexpr (std::is_same_v<T, const cppast::cpp_type*>) {
      return target(*entity);
    }

    logger::error(fmt::format("Template callback `target` not valid here. Cannot create link to {}.", nlohmann::to_string(data)));
    return std::string{};
  }, self->from_json(data));
}

std::string inja_formatter::target(const cppast::cpp_entity& entity) const {
  return self->href_schema + to_json(model::link_target(entity)).dump();
}

std::string inja_formatter::target(const cppast::cpp_type& type) const {
  // TODO
  return std::string{};
}

}
