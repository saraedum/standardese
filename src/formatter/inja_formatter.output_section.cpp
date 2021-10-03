// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <fmt/format.h>

#include "../../standardese/logger.hpp"
#include "../../standardese/model/visitor/visit.hpp"

#include "inja_formatter.impl.hpp"

namespace standardese::formatter {

nlohmann::json inja_formatter::output_section_callback(const nlohmann::json& data) const {
  return std::visit([&](auto&& entity) -> nlohmann::json {
    using T = std::decay_t<decltype(entity)>;
    if constexpr (std::is_same_v<T, const model::entity*>) {
      auto output_section = this->output_section(*entity);
      if (output_section.has_value()) {
        return output_section.value();
      } else {
        return nullptr;
      }
    }
    logger::error(fmt::format("Template callback `output_section` not valid here. Cannot determine output section for {}.", self->to_string(data)));
    return nullptr;
  }, self->from_json(data));
}

type_safe::optional<std::string> inja_formatter::output_section(const model::entity& entity) const {
  return model::visitor::visit([&](auto&& documentation) {
    using T = std::decay_t<decltype(documentation)>;
    if constexpr (std::is_base_of_v<model::mixin::documentation, T>) {
      return documentation.output_section;
    }
    return type_safe::optional<std::string>{type_safe::nullopt};
  }, entity);
}

}
