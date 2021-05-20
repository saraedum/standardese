// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <fmt/format.h>

#include <cppast/cpp_entity_kind.hpp>
#include <cppast/cpp_class.hpp>
#include <cppast/cpp_template.hpp>

#include "inja_formatter.impl.hpp"
#include "../../standardese/logger.hpp"

namespace standardese::formatter {

std::string inja_formatter::kind_callback(const nlohmann::json& data) const {
  return std::visit([&](auto&& entity) {
    using T = std::decay_t<decltype(entity)>;
    if constexpr (std::is_same_v<T, const cppast::cpp_entity*>) {
      return kind(*entity);
    }

    logger::error(fmt::format("Template callback `kind` not valid here. Cannot determine kind of {}.", nlohmann::to_string(data)));
    return std::string{};
  }, self->from_json(data));
}

std::string inja_formatter::kind(const cppast::cpp_entity& entity) const {
  switch(entity.kind()) {
    case cppast::cpp_entity_kind::file_t:
      return "Header File";
    case cppast::cpp_entity_kind::constructor_t:
      // TODO: (Special) Constructors
      return "Constructor";
    case cppast::cpp_entity_kind::function_t:
    case cppast::cpp_entity_kind::member_function_t:
      // TODO: (Special) Operators
      return "Function";
    case cppast::cpp_entity_kind::function_parameter_t:
      return "Parameter";
    case cppast::cpp_entity_kind::macro_parameter_t:
      return "Macro Parameter";
    case cppast::cpp_entity_kind::macro_definition_t:
      return "Macro";
    case cppast::cpp_entity_kind::include_directive_t:
      return "Inclusion Directive";
    case cppast::cpp_entity_kind::language_linkage_t:
      return "Language Linkage";
    case cppast::cpp_entity_kind::namespace_t:
      return "Namespace";
    case cppast::cpp_entity_kind::namespace_alias_t:
      return "Namespace Alias";
    case cppast::cpp_entity_kind::using_directive_t:
      return "Using Directive";
    case cppast::cpp_entity_kind::using_declaration_t:
      return "Using Declaration";
    case cppast::cpp_entity_kind::type_alias_t:
      return "Type Alias";
    case cppast::cpp_entity_kind::enum_t:
      return "Enumeration";
    case cppast::cpp_entity_kind::enum_value_t:
      return "Enumeration Constant";
    case cppast::cpp_entity_kind::class_t:
      switch (static_cast<const cppast::cpp_class&>(entity).class_kind())
      {
        case cppast::cpp_class_kind::class_t:
          return "Class";
        case cppast::cpp_class_kind::struct_t:
          return "Struct";
        case cppast::cpp_class_kind::union_t:
          return "Union";
      }
      break;
    case cppast::cpp_entity_kind::access_specifier_t:
      return "Access Specifier";
    case cppast::cpp_entity_kind::base_class_t:
      return "Base Class";
    case cppast::cpp_entity_kind::member_variable_t:
      return "Member Variable";
    case cppast::cpp_entity_kind::variable_t:
    case cppast::cpp_entity_kind::bitfield_t:
      return "Variable";
    case cppast::cpp_entity_kind::conversion_op_t:
      return "Conversion Operator";
    case cppast::cpp_entity_kind::destructor_t:
      return "Destructor";
    case cppast::cpp_entity_kind::friend_t:
      return "Friend Function";
    case cppast::cpp_entity_kind::template_type_parameter_t:
    case cppast::cpp_entity_kind::non_type_template_parameter_t:
    case cppast::cpp_entity_kind::template_template_parameter_t:
      return "Template Parameter";
    case cppast::cpp_entity_kind::alias_template_t:
      return "Alias Template";
    case cppast::cpp_entity_kind::variable_template_t:
      return "Variable template";
    case cppast::cpp_entity_kind::function_template_t:
    case cppast::cpp_entity_kind::function_template_specialization_t:
    case cppast::cpp_entity_kind::class_template_t:
    case cppast::cpp_entity_kind::class_template_specialization_t:
      return kind(*static_cast<const cppast::cpp_template&>(entity).begin());
    case cppast::cpp_entity_kind::static_assert_t:
      return "Static Assertion";
    case cppast::cpp_entity_kind::unexposed_t:
      return "Unexposed Entity";
    case cppast::cpp_entity_kind::count:
      throw std::logic_error("Unsupported entity kind.");
  }
}

}
