// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <fmt/format.h>
#include <unordered_map>
#include <stack>

#include <cppast/cpp_entity.hpp>
#include <cppast/visitor.hpp>

#include "../../standardese/transformation/group_uncommented_transformation.hpp"
#include "../../standardese/model/visitor/visit.hpp"
#include "../../standardese/logger.hpp"

// TODO: This does not work in this case currently:
// ==* GROUP *==
// void f();
// void g();
//
// Since the first entry is already uncommented.

namespace standardese::transformation {

namespace {

bool groups(cppast::cpp_entity_kind kind) {
  switch (kind) {
    case cppast::cpp_entity_kind::enum_value_t:
    case cppast::cpp_entity_kind::function_t:
    case cppast::cpp_entity_kind::function_template_t:
    case cppast::cpp_entity_kind::member_function_t:
    case cppast::cpp_entity_kind::conversion_op_t:
    case cppast::cpp_entity_kind::constructor_t:
    case cppast::cpp_entity_kind::friend_t:
      return true;
    case cppast::cpp_entity_kind::destructor_t:
      // Do not automatically group the destructor as it
      // typically undocumented and the intention was probably
      // just to exclude it from the output.
      return false;
    case cppast::cpp_entity_kind::enum_t:
    case cppast::cpp_entity_kind::class_template_t:
    case cppast::cpp_entity_kind::class_t:
      // Do not group types automatically as this is usually not
      // what users expect. Instead, uncommented (inner) types
      // were probably meant to be hidden from the output.
      return false;
    case cppast::cpp_entity_kind::member_variable_t:
    case cppast::cpp_entity_kind::file_t:
    case cppast::cpp_entity_kind::macro_parameter_t:
    case cppast::cpp_entity_kind::macro_definition_t:
    case cppast::cpp_entity_kind::include_directive_t:
    case cppast::cpp_entity_kind::language_linkage_t:
    case cppast::cpp_entity_kind::namespace_t:
    case cppast::cpp_entity_kind::namespace_alias_t:
    case cppast::cpp_entity_kind::using_directive_t:
    case cppast::cpp_entity_kind::using_declaration_t:
    case cppast::cpp_entity_kind::type_alias_t:
    case cppast::cpp_entity_kind::access_specifier_t:
    case cppast::cpp_entity_kind::base_class_t:
    case cppast::cpp_entity_kind::variable_t:
    case cppast::cpp_entity_kind::bitfield_t:
    case cppast::cpp_entity_kind::function_parameter_t:
    case cppast::cpp_entity_kind::template_type_parameter_t:
    case cppast::cpp_entity_kind::non_type_template_parameter_t:
    case cppast::cpp_entity_kind::template_template_parameter_t:
    case cppast::cpp_entity_kind::alias_template_t:
    case cppast::cpp_entity_kind::variable_template_t:
    case cppast::cpp_entity_kind::function_template_specialization_t:
    case cppast::cpp_entity_kind::class_template_specialization_t:
    case cppast::cpp_entity_kind::static_assert_t:
      // Do not implicitly group things that people usually
      // don't want to be grouped or that we do not generate comments for anyway.
      return false;
    default:
      logger::error("TODO: unknown kind");
      return false;
  }
}

}

group_uncommented_transformation::group_uncommented_transformation(model::unordered_entities& documents, group_uncommented_options options) : transformation(documents), options(std::move(options)) {}

void group_uncommented_transformation::do_transform(model::entity& document) {
  // We will assign groups to cpp_entity_documentation entities if the entity
  // preceding them in the C++ source code (not in this document) has a group
  // and if that entity lives in the same document (we cannot group entities in
  // different output documents.)

  // Whether an entity has empty documentation in this document.
  // Only entities that have any documentation in this document at all show up
  // in this mapping.
  std::unordered_map<const cppast::cpp_entity*, bool> uncommented;

  {
    std::stack<bool> empty;
    empty.push(true);

    model::visitor::visit([&](auto&& entity, auto&& recurse) {
      using T = std::decay_t<decltype(entity)>;

      if constexpr (std::is_same_v<T, model::markup::block_quote>
        || std::is_same_v<T, model::markup::code_block>
        || std::is_same_v<T, model::markup::code>
        || std::is_same_v<T, model::markup::emphasis>
        || std::is_same_v<T, model::markup::hard_break>
        || std::is_same_v<T, model::markup::heading>
        || std::is_same_v<T, model::markup::image>
        || std::is_same_v<T, model::markup::link>
        || std::is_same_v<T, model::markup::list>
        || std::is_same_v<T, model::markup::list_item>
        || std::is_same_v<T, model::markup::paragraph>
        || std::is_same_v<T, model::markup::soft_break>
        || std::is_same_v<T, model::markup::strong_emphasis>
        || std::is_same_v<T, model::markup::text>
        || std::is_same_v<T, model::markup::thematic_break>)
        empty.top() = false;

      empty.push(true);

      recurse();

      const bool is_empty = empty.top();

      if constexpr (std::is_same_v<T, model::cpp_entity_documentation>) {
        uncommented[&entity.entity()] = is_empty;
      }

      empty.pop();
      empty.top() &= is_empty;
    }, document);
  }

  // The explicitly or implicitly assigned group for the entities documented in
  // this document.
  std::unordered_map<const cppast::cpp_entity*, std::string> group;

  model::visitor::visit([&](auto&& entity, auto&& recurse) {
    using T = std::decay_t<decltype(entity)>;

    if constexpr (std::is_same_v<T, model::cpp_entity_documentation>) {
      if (entity.group.has_value()) {
        group[&entity.entity()] = entity.group.value();
      }

    }
    recurse();
  }, document);

  // Assign groups to uncommented entities.
  model::visitor::visit([&](auto&& entity, auto&& recurse) {
    using T = std::decay_t<decltype(entity)>;

    if constexpr (std::is_same_v<T, model::cpp_entity_documentation>) {
      if (uncommented.at(&entity.entity()) && !entity.group.has_value() && groups(entity.entity().kind())) {
        logger::trace([&]() { return fmt::format("Searching group for uncommented entity {}.", entity.entity().name()); });

        // Search backwards in the C++ source code for a group entity.
        const auto parent = entity.entity().parent();

        const cppast::cpp_entity* previous = nullptr;

        if (parent.has_value()) {
          cppast::visit(parent.value(), [&](const cppast::cpp_entity& sibling, const cppast::visitor_info& info) {
            if (&sibling == &parent.value())
              // Enter the parent node and abort when leaving it.
              return true;

            if (&sibling == &entity.entity()) {
              // previous is the node preceding our entity, copy its group if it has any.
              if (previous != nullptr) {
                if (group.find(previous) != group.end()) {
                  logger::debug([&]() { return fmt::format("Adding {} to group {}.", entity.entity().name(), group.at(previous)); });
                  entity.group = group.at(previous);
                  group[&entity.entity()] = group.at(previous);
                }
              }

              // Abort the search.
              return false;
            }

            switch (info.event) {
              case cppast::visitor_info::container_entity_enter:
              case cppast::visitor_info::leaf_entity:
                if (!groups(sibling.kind())) {
                  // This entity kind breaks the implicit grouping.
                  previous = nullptr;
                } else {
                  if (uncommented.find(&sibling) == uncommented.end()) {
                    // This entity is not present in this document.
                    previous = nullptr; 
                  } else {
                    if (uncommented.at(&sibling)) {
                      // This entity is uncommented. Ignore its group.
                    } else {
                      previous = &sibling;
                    }
                  }
                }
                break;
              case cppast::visitor_info::container_entity_exit:
                break;
            }

            switch (info.event) {
              case cppast::visitor_info::container_entity_enter:
                  // Ignore the children of this container.
                  return false;
              default:
                  // Continue the search.
                  return true;
            }
          });
        }
      }
    }

    recurse();
  }, document);
}

}
