// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <cppast/forward.hpp>
#include <regex>
#include <boost/filesystem/path.hpp>

#include <fmt/format.h>
#include <cppast/visitor.hpp>
#include <cppast/cpp_function_template.hpp>
#include <cppast/cpp_file.hpp>
#include <cppast/cpp_entity_kind.hpp>
#include <cppast/cpp_class_template.hpp>
#include <cppast/cpp_preprocessor.hpp>
#include <cppast/cpp_friend.hpp>

#include "../../standardese/document_builder/entity_document_builder.hpp"
#include "../../standardese/model/document.hpp"
#include "../../standardese/model/visitor/generic_visitor.hpp"
#include "../../standardese/model/visitor/visit.hpp"
#include "../../standardese/model/unordered_entities.hpp"
#include "../../standardese/model/mixin/container.hpp"
#include "../../standardese/logger.hpp"

namespace standardese::document_builder {

namespace {

/// Constructs a tree of documentation nodes under a fixed `root` node.
struct visitor : public model::visitor::generic_visitor<visitor> {
  visitor(model::mixin::container<>& root, const model::unordered_entities& entities);

  /// Add documentation for `entity` to `root`.
  template <typename T>
  void operator()(T&& entity);

  /// Return the `section` section of `parent` or create such a section if it
  /// does not exist yet.
  model::section& ensure_section(model::cpp_entity_documentation& parent, parser::commands::section_command section);

  /// Add documentation for the template parameters of `entity`.
  void add_template_parameters(const cppast::cpp_template& entity);

  /// Add documentation for the function parameters of `entity`.
  void add_function_parameters(const cppast::cpp_function_base& entity);

  /// Add documentation for the macro parameters of `entity`.
  void add_macro_parameters(const cppast::cpp_macro_definition& entity);

  /// Add documentation for the base classes of `entity`.
  void add_bases(const cppast::cpp_entity& entity);

  /// Add documentation for `entity` itself to `root` without considering its
  /// children, parameters, bases, …
  void add_entity(const cppast::cpp_entity& entity);

  /// Add documentation for `entity` and all its children to `root`.
  void add_container(const cppast::cpp_entity& container);

  /// Add documentation for the children of `container` to `under`.
  void add_contents(const cppast::cpp_entity& container, model::mixin::container<>& under);

  /// Add documentation for a friend entity to `root`.
  void add_friend(const cppast::cpp_friend& entity);

  model::mixin::container<>* root;
  const model::unordered_entities& entities;
};

}

model::document entity_document_builder::build(const std::string& name, const std::string& path, const model::entity& entity, const model::unordered_entities& entities) const {
  auto document = model::document(name, path);

  visitor v(document, entities);
  entity.accept(v);

  return document;
}

namespace {
visitor::visitor(model::mixin::container<>& root, const model::unordered_entities& entities) : root(&root), entities(entities) {}

template <typename T>
void visitor::operator()(T&& documentation) {
  using S = std::decay_t<decltype(documentation)>;
  if constexpr (std::is_same_v<S, model::cpp_entity_documentation>) {
    const auto& entity = documentation.entity();

    // Create documentation for entity and its children.
    switch(entity.kind()) {
      case cppast::cpp_entity_kind::file_t:
      case cppast::cpp_entity_kind::namespace_t:
      case cppast::cpp_entity_kind::class_t:
      case cppast::cpp_entity_kind::enum_t:
        add_container(entity);
        break;
      case cppast::cpp_entity_kind::class_template_t:
        add_entity(entity);
        add_contents(static_cast<const cppast::cpp_class_template&>(entity).class_(), root->rbegin()->as<model::mixin::container<>>());
        break;
      case cppast::cpp_entity_kind::function_template_t:
      case cppast::cpp_entity_kind::template_type_parameter_t:
      case cppast::cpp_entity_kind::function_parameter_t:
      case cppast::cpp_entity_kind::macro_parameter_t:
      case cppast::cpp_entity_kind::function_t:
      case cppast::cpp_entity_kind::constructor_t:
      case cppast::cpp_entity_kind::destructor_t:
      case cppast::cpp_entity_kind::conversion_op_t:
      case cppast::cpp_entity_kind::member_function_t:
      case cppast::cpp_entity_kind::type_alias_t:
      case cppast::cpp_entity_kind::member_variable_t:
      case cppast::cpp_entity_kind::variable_t:
      case cppast::cpp_entity_kind::base_class_t:
      case cppast::cpp_entity_kind::macro_definition_t:
      case cppast::cpp_entity_kind::enum_value_t:
      case cppast::cpp_entity_kind::alias_template_t:
        add_entity(entity);
        break;
      case cppast::cpp_entity_kind::language_linkage_t:
        add_contents(entity, *root);
        break;
      case cppast::cpp_entity_kind::using_directive_t:
      case cppast::cpp_entity_kind::include_directive_t:
      case cppast::cpp_entity_kind::using_declaration_t:
      case cppast::cpp_entity_kind::access_specifier_t:
      case cppast::cpp_entity_kind::function_template_specialization_t:
      case cppast::cpp_entity_kind::class_template_specialization_t:
        // Ignore this entity. We might want to change this eventually and actually show these if they have explicit documentation.
        break;
      case cppast::cpp_entity_kind::friend_t:
        add_friend(static_cast<const cppast::cpp_friend&>(entity));
        break;
      default:
        logger::error(fmt::format("Not implemented: cannot generate documentation for entity `{}` of type `{}` yet.", entity.name(), (long)entity.kind()));
        return;
    }

    // Create documentation for template parameters.
    if (cppast::is_template(entity.kind()))
      add_template_parameters(static_cast<const cppast::cpp_template&>(entity));

    // Create documentation for base classes.
    switch(entity.kind()) {
      case cppast::cpp_entity_kind::class_t:
        add_bases(entity);
        break;
      case cppast::cpp_entity_kind::class_template_t:
        add_bases(static_cast<const cppast::cpp_class_template&>(entity).class_());
        break;
    }

    // Create documentation for function parameters.
    switch(entity.kind()) {
      case cppast::cpp_entity_kind::function_template_t:
        add_function_parameters(static_cast<const cppast::cpp_function_template&>(entity).function());
        break;
      case cppast::cpp_entity_kind::function_t:
      case cppast::cpp_entity_kind::member_function_t:
      case cppast::cpp_entity_kind::constructor_t:
      case cppast::cpp_entity_kind::destructor_t:
        add_function_parameters(static_cast<const cppast::cpp_function_base&>(entity));
        break;
      case cppast::cpp_entity_kind::macro_definition_t:
        add_macro_parameters(static_cast<const cppast::cpp_macro_definition&>(entity));
        break;
    }
  } else {
    throw std::logic_error("not implemented: can not create an entity document from this kind of entity yet since it is not a C++ entity.");
  }
}

model::section& visitor::ensure_section(model::cpp_entity_documentation& parent, parser::commands::section_command section) {
  auto search = parent.section(section);
  if (!search)
    parent.add_child(model::section(section));
  return parent.section(section).value();
}

void visitor::add_template_parameters(const cppast::cpp_template& entity) {
  for (const auto& tparam: entity.parameters()) {
    auto& parent = root->rbegin()->as<model::cpp_entity_documentation>();
    auto& section = ensure_section(parent, parser::commands::section_command::requires);

    visitor v(section, entities);

    const auto& search = entities.find_cpp_entity(tparam);
    if (search == entities.end()) {
      logger::warn(fmt::format("Ignoring template parameter `{}` of `{}` since no documentation entity could be found for it, not even an empty one.", tparam.name(), entity.name()));
      continue;
    }
    search->accept(v);
  }
}

void visitor::add_function_parameters(const cppast::cpp_function_base& entity) {
  for (const auto& param: entity.parameters()) {
    auto& parent = root->rbegin()->as<model::cpp_entity_documentation>();
    auto& section = ensure_section(parent, parser::commands::section_command::parameters);

    visitor v(section, entities);

    const auto& search = entities.find_cpp_entity(param);
    if (search == entities.end()) {
      logger::warn(fmt::format("Ignoring parameter `{}` of `{}` since no documentation entity could be found for it, not even an empty one.", param.name(), entity.name()));
      continue;
    }
    search->accept(v);
  }
}

void visitor::add_macro_parameters(const cppast::cpp_macro_definition& entity) {
  for (const auto& param: entity.parameters()) {
    auto& parent = root->rbegin()->as<model::cpp_entity_documentation>();
    auto& section = ensure_section(parent, parser::commands::section_command::parameters);

    visitor v(section, entities);

    const auto& search = entities.find_cpp_entity(param);
    if (search == entities.end()) {
      logger::warn(fmt::format("Ignoring macro parameter `{}` of `{}` since no documentation entity could be found for it, not even an empty one.", param.name(), entity.name()));
      continue;
    }
    search->accept(v);
  }
}

void visitor::add_bases(const cppast::cpp_entity& entity) {
  for (const auto& base: static_cast<const cppast::cpp_class&>(entity).bases()) {
    auto& parent = root->rbegin()->as<model::cpp_entity_documentation>();
    auto& section = ensure_section(parent, parser::commands::section_command::bases);

    visitor v(section, entities);

    const auto& search = entities.find_cpp_entity(base);
    if (search == entities.end()) {
      logger::warn(fmt::format("Ignoring base `{}` of `{}` since no documentation entity could be found for it, not even an empty one.", base.name(), entity.name()));
      continue;
    }
    search->accept(v);
  }
}

void visitor::add_entity(const cppast::cpp_entity& entity) {
    const auto& search = entities.find_cpp_entity(entity);
    if (search == entities.end()) {
      logger::warn(fmt::format("Not adding `{}` to documentation since no documentation entity could be found for it, not even an empty one.", entity.name()));
      return;
    }
    root->add_child(*search);
}

void visitor::add_friend(const cppast::cpp_friend& friend_entity) {
  if (!friend_entity.entity().has_value())
    // Ignore friend declarations that refer to existing types. We do not currently show these in the documentation.
    return;

  const auto& friended_entity = friend_entity.entity().value();

  // Take the node documenting the `friend` entity as a new root node.
  const auto& search = entities.find_cpp_entity(friend_entity);
  if (search == entities.end()) {
    logger::warn(fmt::format("Not adding friend `{}` to documentation since no documentation entity could be found for it, not even an empty one.", friended_entity.name()));
    return;
  }
  model::cpp_entity_documentation root = search->as<cpp_entity_documentation>();

  // Add the node describing the friended entity under this new root.
  {
    const auto& search = entities.find_cpp_entity(friended_entity);
    if (search == entities.end()) {
      logger::warn(fmt::format("Not adding friended `{}` to documentation since no documentation entity could be found for it, not even an empty one.", friended_entity.name()));
      return;
    }

    visitor v(root, entities);
    search->accept(v);
  }

  // We now move the contents of the friended entity to the friend, i.e., we
  // drop the extra layer created by the friended entity itself.
  this->root->add_child(root);
  auto& frend = this->root->rbegin()->as<model::cpp_entity_documentation>();
  frend.clear();
  for (auto& child : root)
    frend.add_child(child);
}

void visitor::add_container(const cppast::cpp_entity& container) {
    // Add this entity to the document and recursively all of its children.
    add_entity(container);
    add_contents(container, root->rbegin()->as<model::mixin::container<>>());
}

void visitor::add_contents(const cppast::cpp_entity& container, model::mixin::container<>& under) {
    visitor v(under, entities);

    // Recursively add the children of this container.
    cppast::visit(container, [&](const cppast::cpp_entity& child, auto info) {
      if (&child == &container)
        // Enter the root container and abort when leaving it.
        return true;

      switch(info.event) {
        case cppast::visitor_info::event_type::container_entity_enter:
          // Do not enter child container, i.e., do not consider our
          // grandchildren. A recursive call will take care of them.
          return false;
        case cppast::visitor_info::event_type::container_entity_exit:
          // We "exit" the container that we did not actually enter in the
          // preceding case. Continue the search.
          [[fallthrough]];
        case cppast::visitor_info::event_type::leaf_entity:
        {
          // Process this entity and continue the visitor.
          const auto& search = entities.find_cpp_entity(child);
          if (search == entities.end()) {
            logger::warn(fmt::format("Ignoring child `{}` of `{}` since no documentation entity could be found for it, not even an empty one.", child.name(), container.name()));
          } else {
            entities.cpp_entity(child).accept(v);
          }
          return true;
        }
        default:
            throw std::logic_error("visitor in unexpected state");
      }
    });
}

}

}
