// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <stack>
#include <vector>
#include <fmt/format.h>

#include "../../standardese/transformation/group_transformation.hpp"

#include "../../standardese/model/group_documentation.hpp"
#include "../../standardese/model/visitor/visit.hpp"
#include "../../standardese/logger.hpp"

namespace standardese::transformation {

group_transformation::group_transformation(model::unordered_entities& documents, group_options options) : transformation(documents), options(std::move(options)) {}

void group_transformation::do_transform(model::entity& document) {
  std::stack<std::vector<model::entity>> containers;
  containers.push({});

  model::visitor::visit([&](auto&& entity, auto&& recurse) {
    using T = std::decay_t<decltype(entity)>;

    if constexpr (std::is_base_of_v<model::mixin::container<>, T>) {
      containers.push({});

      recurse();

      entity.clear();
      for (auto& child : containers.top()) entity.emplace_child(std::move(child));
      containers.pop();
    }

    if constexpr (std::is_same_v<T, model::cpp_entity_documentation>) {
      if (entity.group.has_value()) {
        std::string name = entity.group.value();

        model::entity* group = nullptr;
        for (auto& sibling : containers.top())
          if (sibling.is<model::group_documentation>() && sibling.as<model::group_documentation>().group.value() == name)
            group = &sibling;

        if (group == nullptr) {
          auto group_documentation = model::group_documentation{entity.context()};
          group_documentation.group = name;
          containers.top().push_back(std::move(group_documentation));
          group = &*containers.top().rbegin();
        }

        merge(group->as<model::group_documentation>(), std::move(entity));
        return;
      }
    }

    containers.top().emplace_back(entity);
  }, document);
}

void group_transformation::merge(model::group_documentation& group, model::cpp_entity_documentation&& entity) const {
  if (entity.synopsis.has_value()) {
    if (group.synopsis.has_value() && group.synopsis.value() != entity.synopsis.value())
      logger::warn(fmt::format("Only one entity of a group can define a synopsis. Ignoring synopsis {} of group {}.", group.synopsis.value(), entity.group.value()));
    group.synopsis = entity.synopsis;
  }
  if (entity.output_section.has_value()) {
    if (group.output_section.has_value() && entity.output_section.value() != group.output_section.value())
      logger::warn(fmt::format("Only one entity of a group can define an output section. Ignoring output section {} of group {}.", group.output_section.value(), entity.group.value()));
    group.output_section = entity.output_section;
  }
  if (entity.module.has_value()) {
    if (group.module.has_value() && group.module.value() != entity.module.value())
      logger::warn(fmt::format("Only one entity of a group can define a module. Ignoring module {} of group {}.", group.module.value(), entity.group.value()));
    group.module = entity.module;
  }
  
  for (auto& child : entity) {
    if (child.is<model::section>()) {
      auto& section = child.as<model::section>();
      // Search for any existing section of the same type in the group.
      auto existing = [&]() {
        for (auto existing = group.begin(); existing != group.end(); ++existing)
          if (existing->is<model::section>() && existing->as<model::section>().type == section.type)
            return existing;
        return group.end();
      }();
      if (existing != group.end()) {
        if (existing->as<model::section>().begin() != existing->as<model::section>().end()) {
          if (section.begin() == section.end())
            // Keep the existing section and drop the one coming from this
            // child since it is empty anyway.
            continue;
          // TODO: Use the configured section names.
          logger::warn(fmt::format("Multiple members of the group {} define a non-empty {} section. The sections will show up in the generated documentation but there will be no indication which section came from which group member originally.", entity.group.value(), "?" /* section.type */));
        } else
          // Replace existing section since it is empty.
          group.erase(existing);
      };
    }
    group.add_child(std::move(child));
  }

  entity.clear();

  group.entities.push_back(std::move(entity));
}

}
