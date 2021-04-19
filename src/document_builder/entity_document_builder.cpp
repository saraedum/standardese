// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <sstream>

#include <cppast/visitor.hpp>
#include <cppast/cpp_function_template.hpp>

#include "../../standardese/document_builder/entity_document_builder.hpp"
#include "../../standardese/model/document.hpp"
#include "../../standardese/model/visitor/recursive_visitor.hpp"
#include "../../standardese/model/visitor/generic_visitor.hpp"
#include "../../standardese/model/unordered_entities.hpp"
#include "../../standardese/model/mixin/container.hpp"
#include "../../standardese/logger.hpp"

// TODO: Standardese had a generation_config::flag::inline_doc. Was it actually implemented anywhere? What did it do?
/*
 inline_doc, //< Show documentation of entities like parameters inline in the parent documentation.
*/


namespace standardese::document_builder
{

namespace {

struct visitor : public model::visitor::generic_visitor<visitor, model::visitor::recursive_visitor<true>> {
  visitor(model::mixin::container<>& root, const model::unordered_entities& entities) : root(&root), entities(entities) {}

  template <typename T>
  void operator()(T& entity)
  {
    throw std::logic_error("not implemented: create entity document for this kind of entity");
  }

  void operator()(const model::cpp_entity_documentation& documentation) {
    const auto& entity = documentation.entity();
    switch(entity.kind()) {
      case cppast::cpp_entity_kind::file_t:
      case cppast::cpp_entity_kind::namespace_t:
      case cppast::cpp_entity_kind::class_t:
        add_container(entity);
        break;
      case cppast::cpp_entity_kind::function_template_t:
      case cppast::cpp_entity_kind::template_type_parameter_t:
      case cppast::cpp_entity_kind::function_parameter_t:
      case cppast::cpp_entity_kind::function_t:
      case cppast::cpp_entity_kind::type_alias_t:
      case cppast::cpp_entity_kind::member_variable_t:
      case cppast::cpp_entity_kind::variable_t:
        add_entity(entity);
        break;
      case cppast::cpp_entity_kind::language_linkage_t:
        add_contents(entity, *root);
        break;
      case cppast::cpp_entity_kind::include_directive_t:
      case cppast::cpp_entity_kind::macro_definition_t:
      case cppast::cpp_entity_kind::using_directive_t:
      case cppast::cpp_entity_kind::using_declaration_t:
      case cppast::cpp_entity_kind::access_specifier_t:
        // Ignore these entities.
        break;
      default:
        std::stringstream s;
        s << (long)entity.kind() << std::flush;
        throw std::logic_error("not implemented: unsupported C++ entity `" + entity.name() + "`; " + s.str());
    }

    switch(entity.kind()) {
      case cppast::cpp_entity_kind::function_template_t:
        add_template_parameters(entity);
      default:
        break;
    }

    switch(entity.kind()) {
      case cppast::cpp_entity_kind::function_template_t:
        add_function_parameters(static_cast<const cppast::cpp_function_template&>(entity).function());
        break;
      case cppast::cpp_entity_kind::function_t:
        add_function_parameters(static_cast<const cppast::cpp_function&>(entity));
        break;
      default:
        break;
    }

    switch(entity.kind()) {
      default:
        break;
    }
  }

  void add_template_parameters(const cppast::cpp_entity& entity) {
    auto parent = root->rbegin()->as<model::cpp_entity_documentation>();
    auto section = parent.section(parser::commands::section_command::requires);

    if (!section)
      throw std::logic_error("not implemented: no template parameters section found");

    visitor v(section.value(), entities);
    for (const auto& tparam: static_cast<const cppast::cpp_template&>(entity).parameters())
      entities.cpp_entity(tparam).accept(v);
  }

  void add_function_parameters(const cppast::cpp_entity& entity) {
    auto parent = root->rbegin()->as<model::cpp_entity_documentation>();
    auto section = parent.section(parser::commands::section_command::parameters);

    if (!section)
      throw std::logic_error("not implemented: no parameters section found");

    visitor v(section.value(), entities);
    for (const auto& param: static_cast<const cppast::cpp_function_base&>(entity).parameters())
      entities.cpp_entity(param).accept(v);
  }

  void add_bases(const cppast::cpp_entity& entity) {
    // Create a section describing the bases.
    throw std::logic_error("not implemented: add bases");
  }

  void add_entity(const cppast::cpp_entity& entity) {
      root->add_child(entities.cpp_entity(entity));
  }

  void add_container(const cppast::cpp_entity& container) {
      // Add this entity to the document and recursively all of its children.
      add_entity(container);
      add_contents(container, root->rbegin()->as<model::mixin::container<>>());
  }

  void add_contents(const cppast::cpp_entity& container, model::mixin::container<>& under) {
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
            // Process this entity and continue the visitor.
            entities.cpp_entity(child).accept(v);
            return true;
          default:
              throw std::logic_error("visitor in unexpected state");
        }
      });
  }
  
  model::mixin::container<>* root;
  const model::unordered_entities& entities;
};

}

model::document entity_document_builder::operator()(const model::entity& entity, const model::unordered_entities& entities) const {
  // TODO: Determine output_name (use entity.output_name if set.)
  // TODO: Make configurable.
  // TODO: Treat other entities.
  std::string name = entity.as<model::cpp_entity_documentation>().entity().name();
  if (name.find_last_of('/') != std::string::npos)
    name = name.substr(name.find_last_of('/') + 1);
  for (auto& c : name) if (c == '.') c = '_';
  name = "doc_" + name;

  auto document = model::document(name);

  document.path = name;

  visitor v(document, entities);
  entity.accept(v);
  return document;
}

}
