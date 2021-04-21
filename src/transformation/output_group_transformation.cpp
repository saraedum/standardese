// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <cppast/code_generator.hpp>
#include <cppast/cpp_class.hpp>
#include <stack>

#include "../../standardese/transformation/output_group_transformation.hpp"
#include "../../standardese/model/visitor/generic_visitor.hpp"
#include "../../standardese/model/entity.hpp"
#include "../../standardese/model/cpp_entity_documentation.hpp"

namespace standardese::transformation {
namespace {

// TODO: Move implementation to the bottom and only define the class here.
struct visitor : model::visitor::generic_visitor<visitor, model::visitor::visitor<false>> {
  visitor() {
    containers.push({});
  }

  template <typename T>
  void operator()(T& entity) {
    static_assert(std::is_same_v<T, std::decay_t<T>>);

    if constexpr (std::is_same_v<T, model::cpp_entity_documentation>) {
      if (entity.output_section.has_value() && !containers.empty()) {
        // TODO: Don't assume that each entity starts with a heading here.
        int level = entity.begin()->template as<model::markup::heading>().level + delta - has_output_section.top();

        if (!has_output_section.top())
          delta++;

        containers.top().emplace_back(model::markup::heading(level, "", entity.output_section.value()));
        has_output_section.top() = true;
      }
    }

    if constexpr (std::is_same_v<T, model::markup::heading>) {
      // TODO: Cap at 5 in a separate transformation.
      entity.level += delta;
    }

    if constexpr (std::is_base_of_v<model::mixin::container<>, T>) {
      containers.push({});
      has_output_section.push(false);

      for (auto& child : entity) child.accept(*this);

      if (has_output_section.top())
        delta--;
      has_output_section.pop();

      entity.clear();
      for (auto& child : containers.top()) entity.emplace_child(std::move(child));

      containers.pop();
    }

    containers.top().emplace_back(entity);
  }

  int delta = 0;
  std::stack<std::vector<model::entity>> containers;
  std::stack<bool> has_output_section;
};

}

void output_group_transformation::do_transform(model::entity& entity) {
  visitor v{};
  entity.accept(v);
}

}

