// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <cppast/cpp_template.hpp>
#include <cppast/cpp_function.hpp>
#include <cppast/cpp_class.hpp>
#include <cppast/cpp_friend.hpp>
#include <cppast/cpp_preprocessor.hpp>
#include <cppast/cpp_function_template.hpp>
#include <cppast/visitor.hpp>

#include "../../standardese/transformer/create_uncommented_child_transformer.hpp"
#include "../../standardese/model/entity.hpp"
#include "../../standardese/model/cpp_entity_documentation.hpp"
#include "../../standardese/model/entity_set.hpp"

namespace standardese::transformer {

create_uncommented_child_transformer::create_uncommented_child_transformer(const model::entity_set* entities, const parser::cpp_context& context) : outer_transformer(entities), context(context) {}

std::vector<model::entity> create_uncommented_child_transformer::do_transform(const model::entity& root) const {
  std::vector<model::entity> created;

  if (root.is<model::cpp_entity_documentation>()) {
    const auto& documentation = root.as<model::cpp_entity_documentation>();

    const auto ensure_child = [&](const auto& cpp_entity) {
      if (entity_set_find(*entities, cpp_entity) == entities->end()) {
        auto documentation = model::cpp_entity_documentation(&cpp_entity, context);
        documentation.exclude_mode = model::exclude_mode::uncommented;
        created.push_back(std::move(documentation));
      }
    };

    const std::function<void(const cppast::cpp_entity&)> ensure_children = [&](const auto& cpp_entity) {
      if (cppast::is_template(cpp_entity.kind())) {
          for (const auto& param : static_cast<const cppast::cpp_template&>(cpp_entity).parameters())
              ensure_child(param);
          if (cpp_entity.kind() == cppast::cpp_entity_kind::function_template_t)
            ensure_children(static_cast<const cppast::cpp_function_template&>(cpp_entity).function());
      }
      if (cppast::is_function(cpp_entity.kind())) {
          for (const auto& param : static_cast<const cppast::cpp_function_base&>(cpp_entity).parameters())
              ensure_child(param);
      }
      if (cpp_entity.kind() == cppast::cpp_macro_definition::kind()) {
          for (const auto& param : static_cast<const cppast::cpp_macro_definition&>(cpp_entity).parameters())
              ensure_child(param);
      }
      if (cpp_entity.kind() == cppast::cpp_class::kind()) {
          for (const auto& base : static_cast<const cppast::cpp_class&>(cpp_entity).bases())
              ensure_child(base);
      }
    };

    ensure_children(documentation.entity());
  }

  return created;
}

}
