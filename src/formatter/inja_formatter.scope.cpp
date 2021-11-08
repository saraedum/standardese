// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <optional>

#include <cppast/cpp_entity_kind.hpp>
#include <cppast/cpp_file.hpp>
#include <cppast/cpp_friend.hpp>
#include <cppast/cpp_type.hpp>
#include <cppast/cpp_array_type.hpp>
#include <cppast/cpp_decltype_type.hpp>
#include <cppast/cpp_function_type.hpp>
#include <cppast/cpp_template.hpp>
#include <cppast/cpp_namespace.hpp>
#include <boost/algorithm/string.hpp>
#include <fmt/format.h>

#include "inja_formatter.impl.hpp"
#include "../../standardese/logger.hpp"

namespace standardese::formatter {

namespace {

/// Return the scopes this entity is defined in. From the outermost top-level
/// scope to the innermost scope such as a function scope.
std::vector<const cppast::cpp_entity*> scopes(const cppast::cpp_entity& entity) {
  const auto& parent = entity.parent();

  if (!parent.has_value()) {
    return {};
  }

  if (entity.kind() == cppast::cpp_entity_kind::friend_t) {
    auto declaration = static_cast<const cppast::cpp_friend&>(parent.value()).entity();
    if (!declaration.has_value()) {
      logger::warn("Template callback `scope` not implemented for type friends. Returning trivial scope.");
      return {};
    }
    if (declaration.value().name().find("::") != std::string::npos) {
      // A friend function declaration lives in the containing namespace unless it has any explicit scope, i.e.,
      // namespace A { class B { friend void f(); } }
      // declares a function A::f. It cannot refer to a function ::f.
      // However, when we write
      // namespace A { class B { friend void C::f(); } }
      // this could refer to lots of things such as A::C::f() or ::C::f().
      // Since cppast does not tell us and it usually does not matter since
      // this does not declare the function itself, we give up in this case.
      logger::warn(fmt::format("Template callback `scope` not implemented for scoped friends. Returning trivial scope for friend declaration {}.", declaration.value().name()));
      return {};
    }

    // A friend function declaration lives in the containing namespace, i.e., no scope.
    return {};
  }

  if (cppast::is_template(parent.value().kind())) {
    return scopes(parent.value());
  }

  switch(parent.value().kind()) {
    case cppast::cpp_entity_kind::namespace_t:
    case cppast::cpp_entity_kind::file_t:
      return {};
    case cppast::cpp_entity_kind::language_linkage_t:
    case cppast::cpp_entity_kind::friend_t:
      return scopes(parent.value());
    case cppast::cpp_entity_kind::class_t:
    {
      auto parent_scopes = scopes(parent.value());
      parent_scopes.push_back(&parent.value());
      return parent_scopes;
    }
    default:
      logger::warn(fmt::format("Template callback `scope` not implemented for the {} parent {} of the {} {}. Returning trivial scope.", cppast::to_string(parent.value().kind()), parent.value().name(), cppast::to_string(entity.kind()), entity.name()));
      return {};
  }
}

/// Return the scopes this type is defined in. From the outermost top-level
/// scope to the innermost scope such as a function scope.
type_safe::optional<std::vector<const cppast::cpp_entity*>> scopes(const cppast::cpp_type& type, const cppast::cpp_entity_index& index) {
  switch (type.kind()) {
    case cppast::cpp_type_kind::array_t:
      return scopes(static_cast<const cppast::cpp_array_type&>(type).value_type(), index);
    case cppast::cpp_type_kind::auto_t:
    case cppast::cpp_type_kind::builtin_t:
    case cppast::cpp_type_kind::decltype_auto_t:
    case cppast::cpp_type_kind::decltype_t:
    case cppast::cpp_type_kind::function_t:
    case cppast::cpp_type_kind::member_function_t:
    case cppast::cpp_type_kind::member_object_t:
    case cppast::cpp_type_kind::template_parameter_t:
      return std::vector<const cppast::cpp_entity*>{};
    case cppast::cpp_type_kind::template_instantiation_t:
    {
      const auto declaration = static_cast<const cppast::cpp_template_instantiation_type&>(type).primary_template().get(index);
      if (declaration.begin() != declaration.end())
        return scopes(declaration.begin()->get());
      break;
    }
    case cppast::cpp_type_kind::cv_qualified_t:
      return scopes(static_cast<const cppast::cpp_cv_qualified_type&>(type).type(), index);
    case cppast::cpp_type_kind::pointer_t:
    case cppast::cpp_type_kind::reference_t:
      return scopes(static_cast<const cppast::cpp_pointer_type&>(type).pointee(), index);
    case cppast::cpp_type_kind::unexposed_t:
      break;
    case cppast::cpp_type_kind::dependent_t:
      // TODO: Can we do better here?
      logger::warn(fmt::format("Not implemented: cannot determine scope() of dependent type {}.", cppast::to_string(type)));
      break;
    case cppast::cpp_type_kind::user_defined_t:
    {
      const auto& user_defined = static_cast<const cppast::cpp_user_defined_type&>(type);
      const auto definition = user_defined.entity().get(index);
      if (definition.begin() != definition.end())
        return scopes(definition.begin()->get());
      break;
    }
    default:
      // TODO
      logger::warn(fmt::format("Not implemented: cannot determine scope() of type {}.", cppast::to_string(type)));
      break;
  }
  return type_safe::nullopt;
}

std::string render_scope(std::vector<const cppast::cpp_entity*> self, std::optional<std::vector<const cppast::cpp_entity*>> context, std::function<std::string(const cppast::cpp_entity&)> name, enum inja_formatter::inja_formatter_options::scope_display_options options) {
  switch (options) {
    case inja_formatter::inja_formatter_options::scope_display_options::hidden:
      return std::string{};
    case inja_formatter::inja_formatter_options::scope_display_options::full:
      context = {};
      break;
    case inja_formatter::inja_formatter_options::scope_display_options::relative:
      break;
  }

  std::string ret;

  for (int i = 0; i < self.size(); i++) {
    if (context.has_value() && i < context->size() && context.value()[i] == self[i])
      continue;
    if (ret.size())
      ret += "::";
    ret += name(*self[i]);
  }

  return ret;
}

}

std::string inja_formatter::scope_callback(const nlohmann::json& data) const {
  return std::visit([&](auto&& entity) {
    using T = std::decay_t<decltype(entity)>;
    if constexpr (std::is_same_v<T, const cppast::cpp_entity*>) {
      return scope(*entity);
    } else if constexpr (std::is_same_v<T, const cppast::cpp_type*>) {
      return scope(*entity);
    } else if constexpr (std::is_same_v<T, const nlohmann::json::string_t*>) {
      return scope(*entity);
    }

    logger::error(fmt::format("Template callback `scope` not valid here. Cannot determine scope of {}.", nlohmann::to_string(data)));
    return std::string{};
  }, self->from_json(data));
}

std::string inja_formatter::scope(const cppast::cpp_entity& entity) const {
  auto entity_scopes = scopes(entity);

  const auto name = [&](const cppast::cpp_entity& type) { return this->name(type); };

  if (self->context.has_value()) {
    // TODO: Do not add context to scope if it does not define a scope.
    auto context_scopes = scopes(self->context.value());
    context_scopes.push_back(&self->context.value());
    return render_scope(entity_scopes, context_scopes, name, self->options.scope_display_options);
  }
  return render_scope(entity_scopes, {}, name, self->options.scope_display_options);
}

std::string inja_formatter::scope(const cppast::cpp_type& type) const {
  const auto type_scopes = scopes(type, self->cpp_context.index());

  if (!type_scopes.has_value())
    return std::string{};

  const auto name = [&](const cppast::cpp_entity& type) { return this->name(type); };

  if (self->context.has_value()) {
    // TODO: Deduplicate with the above.
    // TODO: Do not add context to scope if it does not define a scope.
    auto context_scopes = scopes(self->context.value());
    context_scopes.push_back(&self->context.value());
    return render_scope(type_scopes.value(), context_scopes, name, self->options.scope_display_options);
  }
  return render_scope(type_scopes.value(), {}, name, self->options.scope_display_options);
}

std::string inja_formatter::scope(const std::string& name) const {
  // We cannot really know what of name is scope and what is namespace, so we
  // just assume that everything is namespace. We could probably do better here
  // by looking at the context's scope and finding a shared prefix.
  return std::string{};
}

}
