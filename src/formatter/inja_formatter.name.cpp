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
#include <cppast/cpp_function_template.hpp>
#include <cppast/cpp_class_template.hpp>
#include <cppast/cpp_template.hpp>
#include <fmt/format.h>
#include <boost/filesystem/path.hpp>
#include <boost/algorithm/string.hpp>

#include "inja_formatter.impl.hpp"
#include "../../standardese/logger.hpp"
#include "../../standardese/model/visitor/visit.hpp"

#include "../../standardese/model/group_documentation.hpp"

namespace standardese::formatter {

std::string inja_formatter::name_callback(const nlohmann::json& data) const {
  return std::visit([&](auto&& entity) {
    using T = std::decay_t<decltype(entity)>;
    if constexpr (std::is_same_v<T, const cppast::cpp_entity*>) {
      return name(*entity);
    } else if constexpr (std::is_same_v<T, const model::entity*>) {
      return model::visitor::visit([&](auto&& entity) {
        using S = std::decay_t<decltype(entity)>;

        if constexpr (std::is_same_v<S, model::module>) {
          return name(entity);
        }

        logger::error(fmt::format("Template callback `name` not valid here. Cannot determine name of {}.", self->to_string(data)));
        return std::string{};
      }, *entity);
    } else if constexpr (std::is_same_v<T, const cppast::cpp_type*>) {
      return name(*entity);
    } else if constexpr (std::is_same_v<T, const nlohmann::json::string_t*>) {
      return name(*entity);
    }

    logger::error(fmt::format("Template callback `name` not valid here. Cannot determine name of {}.", self->to_string(data)));
    return std::string{};
  }, self->from_json(data));
}

std::string inja_formatter::name(const cppast::cpp_entity& entity) const {
  switch(entity.kind()) {
    case cppast::cpp_entity_kind::function_template_t:
      return name(static_cast<const cppast::cpp_function_template&>(entity).function());
    case cppast::cpp_entity_kind::class_template_t:
      return name(static_cast<const cppast::cpp_class_template&>(entity).class_());
    case cppast::cpp_entity_kind::file_t:
      return boost::filesystem::path(entity.name()).filename().native();
    case cppast::cpp_entity_kind::friend_t:
    {
      const auto& frend = static_cast<const cppast::cpp_friend&>(entity);
      if (frend.entity().has_value())
        return name(frend.entity().value());
      logger::warn(fmt::format("Could not determine entity underlying a friend. Will return `{}` as its name.", frend.name()));
      return frend.name();
    }
    case cppast::cpp_entity_kind::function_t:
    case cppast::cpp_entity_kind::member_function_t:
    case cppast::cpp_entity_kind::class_t:
    case cppast::cpp_entity_kind::constructor_t:
    case cppast::cpp_entity_kind::destructor_t:
    case cppast::cpp_entity_kind::conversion_op_t:
    case cppast::cpp_entity_kind::function_parameter_t:
    case cppast::cpp_entity_kind::template_type_parameter_t:
      // If the fnuction parameter is unnamed, this returns the empty string.
    case cppast::cpp_entity_kind::variable_t:
    case cppast::cpp_entity_kind::member_variable_t:
    case cppast::cpp_entity_kind::type_alias_t:
    case cppast::cpp_entity_kind::enum_t:
    case cppast::cpp_entity_kind::enum_value_t:
      return entity.name();
    default:
      // TODO: Make sure that this is complete and every case tested.
      logger::warn(fmt::format("Not implemented: Cannot determine name of a {} yet. Will return {} instead.", cppast::to_string(entity.kind()), entity.name()));
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
      return name(static_cast<const cppast::cpp_unexposed_type&>(type).name());
    case cppast::cpp_type_kind::user_defined_t:
      // TDOO: We can probably do better here, cf. scope()/namespaze().
      return name(static_cast<const cppast::cpp_user_defined_type&>(type).entity().name());
    default:
      // TODO: Make sure that this is complete and every case tested.
      throw std::logic_error("not implemented: name for unexpected type");
  }
}

/// Remove any leading `scope::` from `name`.
/// It is tricky to get this right in general. We could assume that no name
/// starts with `::` which is hopefully what libclang provides us with. This
/// allows us to decide whether `>` ends a template arguments list or is a
/// comparison operator. However, there's still no easy way to decide the
/// corresopnding question for `<`.
/// Instead, we only get this right in trivial cases.
std::string inja_formatter::name(const std::string& name) const {
  {
    // The nesting depth of the `<`.
    int depth = 0;

    // Walk the name from the front, searching for a top-level ::.
    for (size_t prefix = 0; prefix + 1 < name.size(); prefix++) {
      if (depth == 0 && name[prefix] == ':' && name[prefix + 1] == ':')
        return this->name(name.substr(prefix + 2));
      if (name[prefix] == '<')
        depth++;
      if (name[prefix] == '>')
        depth--;

      if (depth < 0) {
        logger::error(fmt::format("Failed to strip scope from {}. Found an unexpected sequence of < and >.", name));
        break;
      }

      if (depth >= 2)
        // Sequence of < and > too complicated for us to safely decide how things are nested.
        break;
    }
  }

  {
    int depth = 0;

    // Walk the name from the back, searching for a top-level ::.
    for (size_t suffix = name.size() - 1; suffix >= 1; suffix--) {
      if (depth == 0 && name[suffix] == ':' && name[suffix - 1] == ':')
        return name.substr(suffix + 1);
      if (name[suffix] == '>')
        depth++;
      if (name[suffix] == '<')
        depth--;

      if (depth < 0) {
        logger::error(fmt::format("Failed to strip scope from {}. Found an unexpected sequence of < and >.", name));
        break;
      }

      if (depth >= 2)
        // Sequence of < and > too complicated for us to safely decide how things are nested.
        break;
    }
  }

  return name;
}

std::string inja_formatter::name(const model::module& module) const {
  return module.name;
}

}
