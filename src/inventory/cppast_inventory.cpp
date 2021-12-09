// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <cassert>
#include <cppast/cpp_entity.hpp>
#include <cppast/cpp_file.hpp>
#include <fmt/format.h>

#include "../../standardese/inventory/cppast_inventory.hpp"
#include "../../standardese/inventory/symbols.hpp"
#include "../../standardese/logger.hpp"
#include "../../standardese/model/entity_set.hpp"
#include "../../standardese/model/visitor/visit.hpp"

namespace standardese::inventory
{

namespace {
const cppast::cpp_entity* find(const std::string& name, const symbols& symbols, const cppast::cpp_entity* entity) {
  const auto target = entity != nullptr ? symbols.findRelative(name, *entity) : symbols.find(name);

  if (!target.has_value())
    return nullptr;

  return target.value().accept([&](auto&& target) -> const cppast::cpp_entity* {
    using T = std::decay_t<decltype(target)>;
    if constexpr (std::is_same_v<T, model::link_target::cppast_target>) {
      return target.target;
    } else {
      logger::warn(fmt::format("Lookup of {} in dictionary of symbols did not produce a cppast entity.", name));
      return nullptr;
    }
  });
}

}

cppast_inventory::cppast_inventory(std::vector<const cppast::cpp_entity*> entities, const parser::cpp_context& context) : context(context) {
  for (const auto& e : entities)
    roots.insert(&root(*e));
}

cppast_inventory::cppast_inventory(const model::entity_set* entities, const parser::cpp_context& context): context(context) {
  for (const auto& entity : *entities) {
    model::visitor::visit([&](auto&& entity, auto&& recurse) {
      using T = std::decay_t<decltype(entity)>;
      if constexpr (std::is_base_of_v<model::cpp_entity_documentation, T>)
        roots.insert(&root(entity.entity()));
      recurse();
    }, entity);
  }
}

const cppast::cpp_file& cppast_inventory::root(const cppast::cpp_entity& entity_) {
  const auto* entity = &entity_;

  while (entity->kind() != cppast::cpp_file::kind())
    entity = &entity->parent().value();
  return static_cast<const cppast::cpp_file&>(*entity);
}

const cppast::cpp_entity* cppast_inventory::find(const std::string& name, const symbols& symbols, const cppast::cpp_entity& entity) {
  return ::standardese::inventory::find(name, symbols, &entity);
}

const cppast::cpp_entity* cppast_inventory::find(const std::string& name, const symbols& symbols) {
  return ::standardese::inventory::find(name, symbols, nullptr);
}

}
