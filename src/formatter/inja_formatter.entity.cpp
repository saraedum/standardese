// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <fmt/format.h>

#include <cppast/cpp_entity_kind.hpp>
#include <cppast/cpp_friend.hpp>
#include <cppast/cpp_function_template.hpp>

#include "inja_formatter.impl.hpp"
#include "../../standardese/logger.hpp"
#include "../../standardese/model/visitor/visit.hpp"

namespace standardese::formatter {

nlohmann::json inja_formatter::entity_callback(const nlohmann::json& data) const {
  return std::visit([&](auto&& entity) {
    using T = std::decay_t<decltype(entity)>;
    if constexpr (std::is_same_v<T, const cppast::cpp_entity*>) {
      switch(entity->kind()) {
        case cppast::cpp_entity_kind::friend_t:
        case cppast::cpp_entity_kind::function_template_t:
          return to_json(&this->entity(*entity));
      }
    } else if constexpr (std::is_same_v<T, const model::entity*>) {
      return model::visitor::visit([&](auto&& documentation) {
        using S = std::decay_t<decltype(documentation)>;
        if constexpr (std::is_same_v<S, model::cpp_entity_documentation>) {
          return to_json(&this->entity(documentation));
        } else if constexpr (std::is_same_v<S, model::group_documentation>) {
          nlohmann::json encoded = nlohmann::json::array();
          for (auto&& child : this->entity(documentation))
            encoded.push_back(to_json(child));
          return encoded;
        }
        logger::error(fmt::format("Template callback `entity` not valid here. Cannot determine underlying entity for {}.", self->to_string(data)));
        return nlohmann::json{};
      }, *entity);
    }
    logger::error(fmt::format("Template callback `entity` not valid here. Cannot determine underlying entity for {}.", self->to_string(data)));
    return nlohmann::json{};
  }, self->from_json(data));
}

const cppast::cpp_entity& inja_formatter::entity(const cppast::cpp_entity& entity) const {
  switch (entity.kind()) {
    case cppast::cpp_entity_kind::friend_t:
      // TODO(0.6.0-rc): When is value not available?
      return static_cast<const cppast::cpp_friend&>(entity).entity().value();
    case cppast::cpp_entity_kind::function_template_t:
      return static_cast<const cppast::cpp_function_template&>(entity).function();
    default:
      logger::error(fmt::format("Entity {} has no underlying entity. Cannot use template callback `entity()` in this context.", entity.name()));
      throw std::logic_error("Entity has no underlying entity.");
  }
}

const cppast::cpp_entity& inja_formatter::entity(const model::cpp_entity_documentation& entity) const {
  return entity.entity();
}

const std::vector<model::cpp_entity_documentation>& inja_formatter::entity(const model::group_documentation& group) const {
  return group.entities;
}

}
