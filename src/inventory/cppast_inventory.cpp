// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <cassert>
#include <cppast/cpp_entity.hpp>
#include <cppast/cpp_file.hpp>

#include "../../standardese/inventory/cppast_inventory.hpp"
#include "../../standardese/inventory/symbols.hpp"

namespace standardese::inventory
{

cppast_inventory::cppast_inventory(std::vector<const cppast::cpp_entity*> entities, const parser::cpp_context& context) : context(context) {
  for (const auto& e : entities)
      roots.insert(&root(*e));
}

const cppast::cpp_file& cppast_inventory::root(const cppast::cpp_entity& entity_) {
  const auto* entity = &entity_;

  while (entity->kind() != cppast::cpp_file::kind())
    entity = &entity->parent().value();
  return static_cast<const cppast::cpp_file&>(*entity);
}

type_safe::optional_ref<const cppast::cpp_entity> cppast_inventory::find(const std::string& name, const cppast::cpp_entity& entity, const parser::cpp_context& context) {
  const auto anchor = symbols(cppast_inventory({&entity}, context)).find(name, entity);

  if (!anchor.has_value())
    return type_safe::nullopt;

  return anchor.value().accept([&](auto&& target) -> type_safe::object_ref<const cppast::cpp_entity> {
    using T = std::decay_t<decltype(target)>;
    if constexpr (std::is_same_v<T, model::link_target::cppast_target>) {
      return target.target;
    } else {
      throw std::logic_error("lookup in cppast_inventory returned something that is not a cppast entity");
    }
  });
}

}
