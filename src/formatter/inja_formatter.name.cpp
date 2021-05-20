// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <cppast/cpp_entity_kind.hpp>
#include <cppast/cpp_file.hpp>
#include <cppast/cpp_friend.hpp>
#include <cppast/cpp_type.hpp>
#include <cppast/cpp_array_type.hpp>
#include <cppast/cpp_decltype_type.hpp>
#include <cppast/cpp_function_type.hpp>
#include <cppast/cpp_template.hpp>
#include <cppast/forward.hpp>
#include <fmt/format.h>
#include <boost/filesystem/path.hpp>

#include "inja_formatter.impl.hpp"
#include "../../standardese/logger.hpp"

#include "../../standardese/model/group_documentation.hpp"

namespace standardese::formatter {

std::string inja_formatter::name_callback(const nlohmann::json& data) const {
  return std::visit([&](auto&& entity) {
    using T = std::decay_t<decltype(entity)>;
    if constexpr (std::is_same_v<T, const cppast::cpp_entity*>) {
      return name(*entity);
    } else if constexpr (std::is_same_v<T, model::module>) {
      return name(entity);
    } else if constexpr (std::is_same_v<T, const cppast::cpp_type*>) {
      return name(*entity);
    }

    logger::error(fmt::format("Template callback `name` not valid here. Cannot determine name of {}.", nlohmann::to_string(data)));
    return std::string{};
  }, self->from_json(data));
}

std::string inja_formatter::name(const cppast::cpp_entity& entity) const {
  switch(entity.kind()) {
    case cppast::cpp_entity_kind::file_t:
      return boost::filesystem::path(entity.name()).filename().native();
    case cppast::cpp_entity_kind::friend_t:
    {
      const auto& frend = static_cast<const cppast::cpp_friend&>(entity);
      if (frend.entity().has_value())
        return frend.entity().value().name();
      return frend.name();
    }
    default:
      return entity.name();
  }
}

std::string inja_formatter::name(const cppast::cpp_type& type) const {
  switch (type.kind()) {
    case cppast::cpp_type_kind::array_t:
      return name(static_cast<const cppast::cpp_array_type&>(type).value_type()) + "[]";
    case cppast::cpp_type_kind::auto_t:
      return "auto";
    case cppast::cpp_type_kind::builtin_t:
      return cppast::to_string(static_cast<const cppast::cpp_builtin_type&>(type).builtin_type_kind());
    case cppast::cpp_type_kind::cv_qualified_t:
      return name(static_cast<const cppast::cpp_cv_qualified_type&>(type).type());
    case cppast::cpp_type_kind::decltype_auto_t:
      return "decltype(auto)";
    case cppast::cpp_type_kind::decltype_t:
      switch (static_cast<const cppast::cpp_decltype_type&>(type).expression().kind()) {
        case cppast::cpp_expression_kind::literal_t:
          return "decltype(" + static_cast<const cppast::cpp_literal_expression&>(static_cast<const cppast::cpp_decltype_type&>(type).expression()).value() + ")";
        default:
          logger::warn("Cannot determine name of type of decltype(unexposed expression). Will render as `decltype(/* ... */)` instead.");
          return "decltype(/* ... */)";
      }
    case cppast::cpp_type_kind::dependent_t:
      return static_cast<const cppast::cpp_dependent_type&>(type).name();
    case cppast::cpp_type_kind::function_t:
    case cppast::cpp_type_kind::member_function_t:
    case cppast::cpp_type_kind::member_object_t:
      logger::error("Cannot render name of a function type. Will render as `::(/* ... */)` instead.");
      return "::(/* ... */)";
    case cppast::cpp_type_kind::pointer_t:
      return name(static_cast<const cppast::cpp_pointer_type&>(type).pointee()) + "*";
    case cppast::cpp_type_kind::reference_t:
      return name(static_cast<const cppast::cpp_pointer_type&>(type).pointee()) + "&";
    case cppast::cpp_type_kind::template_instantiation_t:
      return static_cast<const cppast::cpp_template_instantiation_type&>(type).primary_template().name();
    case cppast::cpp_type_kind::template_parameter_t:
      return static_cast<const cppast::cpp_template_parameter_type&>(type).entity().name();
    case cppast::cpp_type_kind::unexposed_t:
      return static_cast<const cppast::cpp_unexposed_type&>(type).name();
    case cppast::cpp_type_kind::user_defined_t:
      return static_cast<const cppast::cpp_user_defined_type&>(type).entity().name();
  }
}

std::string inja_formatter::name(const model::module& module) const {
  return module.name;
}

}
