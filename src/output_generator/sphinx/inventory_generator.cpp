// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <cppast/cpp_language_linkage.hpp>

#include "../../../standardese/output_generator/sphinx/inventory_generator.hpp"
#include "../../../standardese/model/cpp_entity_documentation.hpp"
#include "../../../standardese/model/group_documentation.hpp"
#include "../../../standardese/model/document.hpp"

// TODO: Create a parser::intersphinx::document_set here and then dump it out.

namespace standardese::output_generator::sphinx
{

inventory_generator::inventory_generator(std::ostream& os) : stream_generator(os) {}

inventory_generator::~inventory_generator() {
  out_ << inventory << std::flush;
}

void inventory_generator::visit(document& document) {
  path = document.path;

  stream_generator::visit(document);
}

void inventory_generator::visit(cpp_entity_documentation& documentation) {
  const auto& entity = documentation.entity();

  const auto [domain, type] = domain_type(entity);
  
  inventory.entries.emplace_back(name(entity), domain, type, priority(entity), path + "/#" + documentation.id, display_name(entity));

  stream_generator::visit(documentation);
}

void inventory_generator::visit(group_documentation& documentation) {
  for (const auto& entity : documentation.entities) {
    const auto [domain, type] = domain_type(entity.entity());
    
    inventory.entries.emplace_back(name(entity.entity()), domain, type, priority(entity.entity()), path + "/#" + documentation.id, display_name(entity.entity()));

    stream_generator::visit(documentation);
  }
}

int inventory_generator::priority(const cppast::cpp_entity& entity) const {
  // TODO: Any reason to return something else?
  return 0;
}

std::string inventory_generator::display_name(const cppast::cpp_entity& entity) const {
  // TODO: Make configurable
  return entity.name();
}

std::string inventory_generator::name(const cppast::cpp_entity& entity) const {
  // TODO: Fully qualified name
  return entity.name();
}

std::pair<std::string, std::string> inventory_generator::domain_type(const cppast::cpp_entity& entity) const {
  std::string domain = "c++";

  {
    auto parent = type_safe::ref(entity);
    while(true) {
      if (parent->kind() == cppast::cpp_language_linkage::kind()) {
        const auto& linkage = static_cast<const cppast::cpp_language_linkage&>(*parent).name();
        if (linkage == R"("C")") {
          domain = "c";
        } else if (linkage == R"("C++")") {
          ;
        } else {
          throw std::logic_error("not implemented: linkage " + linkage);
        }
        break;
      }

      if (!parent->parent())
        break;

      parent = type_safe::ref(parent->parent().value());
    }
  }

  std::string type;

  switch(entity.kind()) {
    case cppast::cpp_entity_kind::file_t:
    case cppast::cpp_entity_kind::macro_parameter_t:
    case cppast::cpp_entity_kind::include_directive_t:
    case cppast::cpp_entity_kind::language_linkage_t:
    case cppast::cpp_entity_kind::namespace_alias_t:
    case cppast::cpp_entity_kind::using_directive_t:
    case cppast::cpp_entity_kind::using_declaration_t:
    case cppast::cpp_entity_kind::enum_value_t:
    case cppast::cpp_entity_kind::access_specifier_t:
    case cppast::cpp_entity_kind::base_class_t:
    case cppast::cpp_entity_kind::function_parameter_t:
    case cppast::cpp_entity_kind::friend_t:
    case cppast::cpp_entity_kind::template_type_parameter_t:
    case cppast::cpp_entity_kind::non_type_template_parameter_t:
    case cppast::cpp_entity_kind::template_template_parameter_t:
    case cppast::cpp_entity_kind::class_template_specialization_t:
    case cppast::cpp_entity_kind::function_template_specialization_t:
    case cppast::cpp_entity_kind::static_assert_t:
    case cppast::cpp_entity_kind::unexposed_t:
      type = "member"; // no explicit support in Sphinx
      break;
    case cppast::cpp_entity_kind::macro_definition_t:
      type = "macro";
      break;
    case cppast::cpp_entity_kind::namespace_t:
      type = "namespace";
      break;
    case cppast::cpp_entity_kind::alias_template_t:
    case cppast::cpp_entity_kind::class_t:
    case cppast::cpp_entity_kind::type_alias_t:
    case cppast::cpp_entity_kind::class_template_t:
      type = "type";
      break;
    case cppast::cpp_entity_kind::enum_t:
      type = "enum";
      break;
    case cppast::cpp_entity_kind::variable_template_t:
    case cppast::cpp_entity_kind::variable_t:
    case cppast::cpp_entity_kind::member_variable_t:
    case cppast::cpp_entity_kind::bitfield_t:
      type = "var";
      break;
    case cppast::cpp_entity_kind::function_t:
    case cppast::cpp_entity_kind::member_function_t:
    case cppast::cpp_entity_kind::conversion_op_t:
    case cppast::cpp_entity_kind::constructor_t:
    case cppast::cpp_entity_kind::destructor_t:
    case cppast::cpp_entity_kind::function_template_t:
      type = "function";
      break;
    default:
      throw std::logic_error("unsupported entity");
  }

  return {domain, type};
}

}
