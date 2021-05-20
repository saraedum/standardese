// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <cppast/cpp_class.hpp>
#include <stack>

#include "../../standardese/transformation/output_group_transformation.hpp"
#include "../../standardese/model/visitor/visit.hpp"
#include "../../standardese/model/entity.hpp"
#include "../../standardese/model/mixin/documentation.hpp"
#include "../../standardese/parser/markdown_parser.hpp"
#include "../../standardese/model/document.hpp"

namespace standardese::transformation {

void output_group_transformation::do_transform(model::entity& document) {
  std::stack<std::vector<model::entity>> containers;
  containers.push({});

  std::stack<bool> has_output_section;

  int delta = 0;

  std::stack<int> level;
  level.push(1);

  model::visitor::visit([&](auto&& entity, auto&& recurse) {
    using T = std::decay_t<decltype(entity)>;

    if constexpr (std::is_base_of_v<model::mixin::documentation, T>) {
      // TODO: Make configurable
      if (entity.output_section.has_value() && !entity.group.has_value()) {
        if (!has_output_section.top())
          delta++;

        auto heading = model::markup::heading(level.top());
        auto title = parser::markdown_parser{}.parse(entity.output_section.value()).begin()->template as<model::markup::paragraph>();
        // TODO: We do this a lot: Parse and treat it as inline.
        for (auto& child : title) {
          heading.add_child(std::move(child));
        }

        containers.top().emplace_back(heading);
        has_output_section.top() = true;
      }
    }

    if constexpr (std::is_same_v<T, model::markup::heading>) {
      // TODO: Cap at 5 in a separate transformation.
      entity.level += delta;
      if (!has_output_section.top())
        level.top() = entity.level + 1;
    }

    if constexpr (std::is_base_of_v<model::mixin::container<>, T>) {
      containers.push({});
      has_output_section.push(false);
      level.push(level.top());

      recurse();

      if (has_output_section.top())
        delta--;
      level.pop();
      has_output_section.pop();

      entity.clear();
      for (auto& child : containers.top()) entity.emplace_child(std::move(child));

      containers.pop();
    }

    containers.top().emplace_back(entity);
  }, document);
}

}

