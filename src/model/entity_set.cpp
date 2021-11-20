// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <fmt/format.h>
#include <fmt/ostream.h>
#include <initializer_list>

#include "../../standardese/model/entity_set.hpp"
#include "../../standardese/model/entity.hpp"
#include "../../standardese/model/cpp_entity_documentation.hpp"
#include "../../standardese/model/module.hpp"
#include "../../standardese/model/visitor/visit.hpp"
#include "../../standardese/logger.hpp"
#include "../../standardese/output_generator/xml/xml_generator.hpp"

namespace standardese::model {

namespace {

bool is_trivial(const entity& entity) {
  if (entity.is<cpp_entity_documentation>()) {
    if(entity.as<cpp_entity_documentation>().exclude_mode == exclude_mode::uncommented)

      return true;
  } else if (entity.is<module>()) {
    if (entity.as<module>().children.empty())
      return true;
  }
  return false;
}

}

size_t cppast_entity_hash::operator()(const entity& self) const {
  return visitor::visit([&](auto&& entity) {
    using T = std::decay_t<decltype(entity)>;
    // TODO(0.6.0-rc): Handle group_documentation?
    if constexpr (std::is_same_v<T, model::cpp_entity_documentation>) {
      return reinterpret_cast<size_t>(&entity.entity());
    } else if constexpr (std::is_same_v<T, model::module>) {
      return std::hash<std::string>()(entity.name);
    } else {
      return reinterpret_cast<size_t>(self.get());
    }
  }, self);
}

bool cppast_entity_equality::operator()(const entity& lhs, const entity& rhs) const {
  return visitor::visit([&](auto&& lentity) {
      using T = std::decay_t<decltype(lentity)>;
      // TODO(0.6.0-rc): Handle group_documentation?
      if constexpr (std::is_same_v<T, model::cpp_entity_documentation>) {
        return visitor::visit([&](auto&& rentity) {
          using S = std::decay_t<decltype(rentity)>;
          if constexpr (std::is_same_v<S, T>) {
            return &lentity.entity() == &rentity.entity();
          }
          return false;
        }, rhs);
      } else if constexpr (std::is_same_v<T, model::module>) {
        return visitor::visit([&](auto&& rentity) {
          using S = std::decay_t<decltype(rentity)>;
          if constexpr (std::is_same_v<S, T>) {
            return lentity.name == rentity.name;
          }
          return false;
        }, rhs);
      }
      return lhs.get() == rhs.get();
  }, lhs);
}

void entity_set_insert(entity_set& entities, const entity& entity) {
  auto [pos, inserted] = entities.insert(entity);
  if (!inserted) {
    if (is_trivial(*pos))
      const_cast<class entity&>(*pos) = entity;
    else if (!is_trivial(entity))
      logger::warn(fmt::format("Not adding entity {} because an equivalent entity {} was already found in this set.", entity, *pos));
  }
}

void entity_set_insert(entity_set& entities, entity&& entity) {
  auto [pos, inserted] = entities.insert(std::move(entity));
  if (!inserted) {
    if (is_trivial(*pos))
      const_cast<class entity&>(*pos) = std::move(entity);
    else if (!is_trivial(entity))
      logger::warn(fmt::format("Not adding entity {} because an equivalent entity {} was already found in this set.", entity, *pos));
  }
}

entity_set::const_iterator entity_set_find(const entity_set& entities, const cppast::cpp_entity& entity) {
  struct hash {
    size_t operator()(const cppast::cpp_entity& entity) const {
      return reinterpret_cast<size_t>(&entity);
    }
  };

  struct equality {
    bool operator()(const cppast::cpp_entity& lhs, const struct entity& rhs) const {
      // TODO(0.6.0-rc): Search group_documentation?
      return rhs.is<cpp_entity_documentation>() && &rhs.as<cpp_entity_documentation>().entity() == &lhs;
    }
  };

  return entities.find(entity, hash{}, equality{});
}

entity_set::iterator entity_set_find(entity_set& entities, const cppast::cpp_entity& entity) {
  auto const_it = entity_set_find(const_cast<const entity_set&>(entities), entity);
  entity_set::iterator it = const_it;
  return it;
}

entity_set::const_iterator entity_set_find_module(const entity_set& entities, const std::string& name) {
  return entities.find(module{name});
}

entity_set::iterator entity_set_find_module(entity_set& entities, const std::string& name) {
  auto const_it = entity_set_find_module(const_cast<const entity_set&>(entities), name);
  entity_set::iterator it = const_it;
  return it;
}

const entity& entity_set_at(const entity_set& entities, const cppast::cpp_entity& entity) {
  auto it = entity_set_find(entities, entity);

  if (it == entities.end())
    throw std::invalid_argument(fmt::format("entity `{}` not found in entities", entity.name()));

  return *it;
}

entity& entity_set_at(entity_set& entities, const cppast::cpp_entity& entity) {
  return const_cast<class entity&>(entity_set_at(const_cast<const entity_set&>(entities), entity));
}

}
