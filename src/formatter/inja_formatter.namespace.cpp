// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <fmt/format.h>
#include <boost/algorithm/string.hpp>
#include <cppast/cpp_entity.hpp>
#include <cppast/cpp_entity_kind.hpp>
#include <cppast/cpp_type.hpp>
#include <cppast/cpp_array_type.hpp>

#include "inja_formatter.impl.hpp"
#include "../../standardese/logger.hpp"

namespace standardese::formatter {

namespace {

std::string render_namespace(std::vector<std::string> self, std::optional<std::vector<std::string>> context, enum inja_formatter::inja_formatter_options::namespace_display_options options) {
  switch (options) {
    case inja_formatter::inja_formatter_options::namespace_display_options::hidden:
      return std::string{};
    case inja_formatter::inja_formatter_options::namespace_display_options::full:
      context = {};
      break;
    case inja_formatter::inja_formatter_options::namespace_display_options::relative:
      break;
  }

  std::string ret;

  for (int i = 0; i < self.size(); i++) {
    if (context.has_value() && i < context->size() && context.value()[i] == self[i])
      continue;
    if (ret.size())
      ret += "::";
    ret += self[i];
  }

  return ret;
}

}

std::string inja_formatter::namespace_callback(const nlohmann::json& data) const {
  return std::visit([&](auto&& entity) {
    using T = std::decay_t<decltype(entity)>;
    if constexpr (std::is_same_v<T, const cppast::cpp_entity*>) {
      return namespaze(*entity);
    } else if constexpr (std::is_same_v<T, const cppast::cpp_type*>) {
      return namespaze(*entity);
    } else if constexpr (std::is_same_v<T, const nlohmann::json::string_t*>) {
      return namespaze(*entity);
    }

    logger::error(fmt::format("Template callback `namespace` not valid here. Cannot determine namespace of {}.", nlohmann::to_string(data)));
    return std::string{};
  }, self->from_json(data));
}

std::string inja_formatter::namespaze(const cppast::cpp_entity& entity) const {
  auto entity_namespaces = namespaces(entity);

  if (self->context.has_value())
    return render_namespace(entity_namespaces, namespaces(self->context.value()), self->options.namespace_display_options);
  return render_namespace(entity_namespaces, {}, self->options.namespace_display_options);
}


std::string inja_formatter::namespaze(const cppast::cpp_type& type) const {
  const auto type_namespaces = namespaces(type);

  if (!type_namespaces.has_value())
    return std::string{};

  if (self->context.has_value())
    return render_namespace(type_namespaces.value(), namespaces(self->context.value()), self->options.namespace_display_options);
  return render_namespace(type_namespaces.value(), {}, self->options.namespace_display_options);
}

std::string inja_formatter::namespaze(const std::string& fullname) const {
  // We cannot really know what of name is scope and what is namespace, so we
  // just assume that everything is namespace. We could probably do better here
  // by looking at the context's scope and finding a shared prefix.
  const std::string name = this->name(fullname); 

  assert(boost::algorithm::starts_with(fullname, name));

  std::string namespaze = fullname.substr(0, fullname.size() - name.size());

  if (boost::algorithm::ends_with(namespaze, "::"))
    namespaze = namespaze.substr(0, namespaze.size() - 2);

  return namespaze;
}

type_safe::optional<std::vector<std::string>> inja_formatter::namespaces(const cppast::cpp_type& type) const {
  std::vector<std::string> ret;

  switch (type.kind()) {
    case cppast::cpp_type_kind::array_t:
      return namespaces(static_cast<const cppast::cpp_array_type&>(type).value_type());
    case cppast::cpp_type_kind::auto_t:
    case cppast::cpp_type_kind::builtin_t:
    case cppast::cpp_type_kind::decltype_auto_t:
    case cppast::cpp_type_kind::decltype_t:
    case cppast::cpp_type_kind::function_t:
    case cppast::cpp_type_kind::member_function_t:
    case cppast::cpp_type_kind::member_object_t:
    case cppast::cpp_type_kind::template_parameter_t:
      return ret;
    case cppast::cpp_type_kind::template_instantiation_t:
      // TODO: Can we do better here?
      return type_safe::nullopt;
    case cppast::cpp_type_kind::cv_qualified_t:
      return namespaces(static_cast<const cppast::cpp_cv_qualified_type&>(type).type());
    case cppast::cpp_type_kind::dependent_t:
      // TODO: Can we do better here?
      return type_safe::nullopt;
    case cppast::cpp_type_kind::pointer_t:
    case cppast::cpp_type_kind::reference_t:
      return namespaces(static_cast<const cppast::cpp_pointer_type&>(type).pointee());
    case cppast::cpp_type_kind::unexposed_t:
      return type_safe::nullopt;
    case cppast::cpp_type_kind::user_defined_t:
      {
        const auto& user_defined = static_cast<const cppast::cpp_user_defined_type&>(type);
        for (const auto& definition : user_defined.entity().get(self->cpp_context.index()))
          return namespaces(definition.get());
        return std::vector{namespaze(user_defined.entity().name())};
      }
    default:
      // TODO
      throw std::logic_error("not implemented: namespaces() for unexpected type");
  }
}

std::vector<std::string> inja_formatter::namespaces(const cppast::cpp_entity& entity) const {
  const auto& parent = entity.parent();

  std::vector<std::string> ret = parent.has_value() ? namespaces(parent.value()) : std::vector<std::string>{};

  if (entity.kind() == cppast::cpp_entity_kind::namespace_t)
    ret.push_back(entity.name());

  return ret;
}

}

