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
#include "../../standardese/output_generator/xml/xml_generator.hpp"

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

      entity.children.clear();
      for (auto& child : containers.top()) entity.children.emplace_back(std::move(child));
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
  
  for (auto& child : entity.children) {
    if (child.is<model::section>()) {
      auto& section = child.as<model::section>();
      // Search for any existing section of the same type in the group.
      auto existing = [&]() {
        for (auto existing = group.children.begin(); existing != group.children.end(); ++existing)
          if (existing->is<model::section>() && existing->as<model::section>().type == section.type)
            return existing;
        return group.children.end();
      }();
      if (existing != group.children.end()) {
        if (existing->as<model::section>().children.begin() != existing->as<model::section>().children.end()) {
          if (section.children.begin() == section.children.end())
            // Keep the existing section and drop the one coming from this
            // child since it is empty anyway.
            continue;
          // TODO(0.6.0-alpha): This reports all the automatically generated Parameters sections. They are not really empty but contain a trivial cpp_entity_documentation.
          // TODO(0.6.0-alpha): Use the configured section names.
          // logger::warn(fmt::format("Multiple members of the group {} define a non-empty {} section. The sections will show up in the generated documentation but there will be no indication which section came from which group member originally. Namely, we found {} and then {}.", entity.group.value(), section.type, output_generator::xml::xml_generator::xml_generator::render(*existing), output_generator::xml::xml_generator::xml_generator::render(section)));
        } else
          // Replace existing section since it is empty.
          group.children.erase(existing);
      };
    }
    group.children.push_back(std::move(child));
  }

  entity.children.clear();

  group.entities.push_back(std::move(entity));
}

}
