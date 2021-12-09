// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <regex>

#include <fmt/format.h>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/erase.hpp>
#include <cppast/cpp_entity.hpp>
#include <cppast/cpp_file.hpp>
#include <cppast/cpp_entity_kind.hpp>
#include <cppast/cpp_template.hpp>
#include <cppast/cpp_function.hpp>
#include <cppast/cpp_language_linkage.hpp>
#include <cppast/cpp_type_alias.hpp>
#include <cppast/cpp_friend.hpp>
#include <cppast/cpp_preprocessor.hpp>
#include <cppast/cpp_namespace.hpp>
#include <cppast/visitor.hpp>
#include <stdexcept>
#include <optional>

#include "../../standardese/inventory/symbols.hpp"
#include "../../standardese/inventory/cppast_inventory.hpp"
#include "../../standardese/inventory/unique_name_inventory.hpp"
#include "../../standardese/inventory/sphinx/documentation_set.hpp"
#include "../../standardese/formatter/inja_formatter.hpp"
#include "../../standardese/logger.hpp"
#include "../../standardese/model/entity_set.hpp"
#include "../../standardese/model/visitor/visit.hpp"

// TODO(0.6.0-beta): We do not handle friend declarations correctly here. A class can
// declare a friend function that then lives in the surrounding namespace of
// the class. (It seems that friend namespace::f() is not possible, or rather
// only possible if f() has been forward declared anyway.) The old standardese
// had some handling for this.

namespace standardese::inventory {

class symbols::impl {
 public:
  virtual ~impl() {}

  virtual std::optional<model::link_target> find(const std::string& name) const = 0;
  virtual std::optional<model::link_target> find(const std::string& name, const cppast::cpp_entity& entity) const;

  /// Split [name]() into a prefix and suffix and return both.
  /// For example, this splits `a::b::c` as `a` and `b::c`.
  static std::optional<std::pair<std::string, std::string>> split(const std::string& name);

  template <typename T>
  class generic_symbols;

  class cppast_symbols;
  class doxygen_symbols;
  class sphinx_symbols;
  class unique_name_symbols;
};

template <typename T>
class symbols::impl::generic_symbols : public symbols::impl {
 public:
  virtual type_safe::optional_ref<const T> descendant(const T&, const std::string& name) const;
  virtual type_safe::optional_ref<const T> child(const T&, const std::string& name) const = 0;
  virtual type_safe::optional_ref<const T> parameter(const T&, const std::string& name) const = 0;
  virtual type_safe::optional_ref<const T> template_parameter(const T&, const std::string& name) const = 0;
  virtual type_safe::optional_ref<const T> base_class(const T&, const std::string& name) const = 0;
  virtual type_safe::optional_ref<const T> frend(const T&, const std::string& name) const = 0;
};

class symbols::impl::cppast_symbols : public symbols::impl::generic_symbols<cppast::cpp_entity> {
 public:
  cppast_symbols(const cppast_inventory*);

  std::optional<model::link_target> find(const std::string& name) const override;
  std::optional<model::link_target> find(const std::string& name, const cppast::cpp_entity& entity) const override;

  type_safe::optional_ref<const cppast::cpp_entity> child(const cppast::cpp_entity&, const std::string& name) const override;
  type_safe::optional_ref<const cppast::cpp_entity> parameter(const cppast::cpp_entity&, const std::string& name) const override;
  type_safe::optional_ref<const cppast::cpp_entity> template_parameter(const cppast::cpp_entity&, const std::string& name) const override;
  type_safe::optional_ref<const cppast::cpp_entity> base_class(const cppast::cpp_entity&, const std::string& name) const override;
  type_safe::optional_ref<const cppast::cpp_entity> frend(const cppast::cpp_entity&, const std::string& name) const override;

 private:
  bool matches(const cppast::cpp_entity& entity, const std::string& search) const;
  std::string template_parameters(const cppast::cpp_entity& entity) const;
  std::string parameter_names(const cppast::cpp_entity& entity) const;
  std::string signature(const cppast::cpp_entity& entity) const;

  const cppast_inventory* inventory;
};

class symbols::impl::sphinx_symbols : public symbols::impl {
 public:
  sphinx_symbols(const sphinx::documentation_set*);

  std::optional<model::link_target> find(const std::string& name) const override;

 private:
  const sphinx::documentation_set* inventory;
};

class symbols::impl::unique_name_symbols : public symbols::impl {
 public:
  unique_name_symbols(const unique_name_inventory*);

  std::optional<model::link_target> find(const std::string& name) const override;
 private:
  std::unordered_map<std::string, const cppast::cpp_entity*> uniquely_named;
  cppast_symbols cppast_symbols;
};

symbols::symbols(const inventory* inventory) {
  if (inventory == nullptr)
    throw std::invalid_argument("inventory must not be null when creating a symbol table");

  if (dynamic_cast<const cppast_inventory*>(inventory) != nullptr) {
    self = std::make_unique<impl::cppast_symbols>(static_cast<const cppast_inventory*>(inventory));
  } else if (dynamic_cast<const sphinx::documentation_set*>(inventory) != nullptr) {
    self = std::make_unique<impl::sphinx_symbols>(static_cast<const sphinx::documentation_set*>(inventory));
  } else if (dynamic_cast<const unique_name_inventory*>(inventory) != nullptr) {
    self = std::make_unique<impl::unique_name_symbols>(static_cast<const unique_name_inventory*>(inventory));
  } else {
    throw std::logic_error("not implemented: symbols for this type of inventory");
  }
}

symbols::symbols(symbols&& value) : self(std::move(value.self)) {}

symbols::~symbols() {}

std::optional<model::link_target> symbols::find(const std::string& name_) const {
  std::string name = name_;

  // TODO(0.6.0-final): Technically, this is wrong for e.g. a unique name lookup.
  if (boost::starts_with(name, "::"))
    // A name in the global scope. But it does not matter since we are already in the global scope.
    name = name.substr(2);

  if (name.empty())
    return std::nullopt;

  return self->find(name);
}

std::optional<model::link_target> symbols::findRelative(const std::string& name, const cppast::cpp_entity& entity) const {
  if (entity.kind() == cppast::cpp_file::kind())
    return this->find(name);

  // TODO(0.6.0-final): Technically, this is wrong for e.g. a unique name lookup.
  if (boost::starts_with(name, "::"))
    // A name in the global scope. Do not try to look it up relative to entity.
    return this->find(name);

  if (name.empty())
    return std::nullopt;

  return self->find(name, entity);
}

std::optional<model::link_target> symbols::impl::find(const std::string& name, const cppast::cpp_entity&) const {
  return find(name);
}

symbols::impl::cppast_symbols::cppast_symbols(const cppast_inventory* inventory) : inventory(inventory) {}

std::optional<model::link_target> symbols::impl::cppast_symbols::find(const std::string& name) const {
  type_safe::optional_ref<const cppast::cpp_entity> found;
  for (auto* root : inventory->roots) {
    auto search = descendant(*root, name);
    if (search) {
      // TODO(0.6.0-final): The definition here and elsewhere feels hacky. Should we really do it like this?
      if (!found || is_definition(search.value()))
        found = search;
    }
  }

  if (found)
    return model::link_target{&found.value()};

  return std::nullopt;
}

std::optional<model::link_target> symbols::impl::cppast_symbols::find(const std::string& name, const cppast::cpp_entity& entity) const {
  if (inventory->roots.find(&cppast_inventory::root(entity)) == inventory->roots.end())
    throw std::invalid_argument("Cannot look up symbol relative to something not defined in any of the loaded files.");

  auto search = descendant(entity, name);
  if (search.has_value())
      return model::link_target{&search.value()};

  if (!entity.parent().has_value())
    return this->find(name);

  return this->find(name, entity.parent().value());
}

std::optional<std::pair<std::string, std::string>> symbols::impl::split(const std::string& name) {
  // We do not distinguish these two operators at all `.` and `::`. Originally,
  // we used `.` for function arguments and `::` in the usual C++ sense but
  // there seems to be not much of a point in enforcing such rules.
  const static auto separator = std::regex(R"(\.|::)");

  std::smatch match;
  if (std::regex_search(name, match, separator))
    return std::pair{match.prefix(), match.suffix()};

  return std::nullopt;
}

template <typename T>
type_safe::optional_ref<const T> symbols::impl::generic_symbols<T>::descendant(const T& root, const std::string& name) const {
  if (name.empty())
      return type_safe::nullopt;

  // The recursive case: if the name contains `::` or `.`, split the name at
  // this separator and search recursively.
  if (auto split = impl::split(name)) {
    const auto [prefix, suffix] = *split;

    auto child = descendant(root, prefix);
    if (child)
        return descendant(child.value(), suffix);

    return type_safe::nullopt;
  }

  // The base case, lookup name itself in the root entity.
  // We roughly follow C++ rules of name-hiding:
  // * arguments hide template parameters
  // * template parameters hide other symbols
  const auto argument = parameter(root, name);
  if (argument) return argument;

  const auto templ = template_parameter(root, name);
  if (templ) return templ;

  const auto base = base_class(root, name);
  if (base) return base;

  return child(root, name);
}

type_safe::optional_ref<const cppast::cpp_entity> symbols::impl::cppast_symbols::parameter(const cppast::cpp_entity& root_, const std::string& name_) const {
  const cppast::cpp_entity* root = &root_;
  std::string name = name_;

  boost::erase_all(name, " ");

  // If this is templatized unwrap the template.
  if (cppast::is_template(root->kind()))
      root = &*(static_cast<const cppast::cpp_template&>(*root).begin());

  // If this is a function: see if name is the name of one of the arguments.
  if (cppast::is_function(root->kind()))
    for (const auto& param : static_cast<const cppast::cpp_function_base&>(*root).parameters())
      if (param.name() == name)
        return type_safe::ref(param);

  if (root->kind() == cppast::cpp_macro_definition::kind())
    for (const auto& param : static_cast<const cppast::cpp_macro_definition&>(*root).parameters())
      if (param.name() == name)
        return type_safe::ref(param);

  return type_safe::nullopt;
}

type_safe::optional_ref<const cppast::cpp_entity> symbols::impl::cppast_symbols::child(const cppast::cpp_entity& root_, const std::string& name_) const {
  const cppast::cpp_entity* root = &root_;
  std::string name = name_;

  boost::erase_all(name, " ");

  // If this is templatized unwrap the template.
  if (cppast::is_template(root->kind()))
      root = &(*static_cast<const cppast::cpp_template&>(*root).begin());

  type_safe::optional_ref<const cppast::cpp_entity> child;
  cppast::visit(*root, [&](const auto& entity, auto info) {
    if (&entity == root)
      // Enter the root container and abort when leaving it.
      return true;

    if (entity.kind() == cppast::cpp_language_linkage::kind())
      // A linkage does not create a naming scope and we never want to link to it.
      return true;

    switch(info.event) {
      case cppast::visitor_info::event_type::container_entity_enter:
          // Do not consider the children of this container. Here we are only
          // looking for a name without `::` or `.`
          return false;
      case cppast::visitor_info::event_type::container_entity_exit:
          // We "exit" the container that we did not actually enter in the
          // preceding case. Continue the search.
          [[fallthrough]];
      case cppast::visitor_info::event_type::leaf_entity:
          break;
      default:
          throw std::logic_error("visitor in unexpected state");
    }

    if (matches(entity, name) && (!child.has_value() || is_definition(entity)))
      child = type_safe::ref(entity);

    // Continue the search.
    return true;
  });

  // It is customary to write `typedef struct S {} S;` or `typedef struct {}
  // S;` especially in C. Technically, an "S" would refer to the typedef, but
  // this is never what people want, instead we refer to the struct.
  // TODO(0.6.0-beta): How should we do this exactly? Should we run this until we are at a non-trivial typedef?
  if (child && child.value().kind() == cppast::cpp_type_alias::kind()) {
    const auto& alias = static_cast<const cppast::cpp_type_alias&>(child.value());
    const auto& type = alias.underlying_type();
    if (type.kind() == cppast::cpp_type_kind::user_defined_t) {
      child = inventory->context.index().lookup(*static_cast<const cppast::cpp_user_defined_type&>(type).entity().id().begin());
    }
  }

  if (!child) {
    // TODO(0.6.0-beta): Do we handle this case correctly?
    // * Search for A in the context of B.
    // * Search for f in the context of A.
    //
    // namespace NS {
    //   class A;
    // }
    // namespace NS {
    //   class B {
    //     friend void f();
    //   }
    // }

    // When entity is a namespace, check all the friends declared by classes in
    // this namespace.
    child = this->frend(*root, name);
  }

  return child;
}

type_safe::optional_ref<const cppast::cpp_entity> symbols::impl::cppast_symbols::frend(const cppast::cpp_entity& root_, const std::string& name_) const {
  const cppast::cpp_entity* root = &root_;
  std::string name = name_;

  boost::erase_all(name, " ");

  switch(root->kind()) {
    case cppast::cpp_entity_kind::file_t:
    case cppast::cpp_entity_kind::namespace_t:
      // We only search for friends declared by types in a (possibly global) namespaces.
      break;
    default:
    return type_safe::nullopt;
  }

  type_safe::optional_ref<const cppast::cpp_entity> frend;
  cppast::visit(*root, [&](const auto& entity, auto info) {
    if (frend.has_value())
      return false;

    if (&entity == root)
      // Enter the root container and abort when leaving it.
      return true;

    if (entity.kind() == cppast::cpp_language_linkage::kind())
      // Ignore linkage scopes and enter them transparently.
      return true;

    if (entity.kind() == cppast::cpp_namespace::kind())
      // Do not search for friends in nested namepaces unless their unnamed.
      return static_cast<const cppast::cpp_namespace&>(entity).is_anonymous();

    switch(info.event) {
      case cppast::visitor_info::event_type::container_entity_enter:
      case cppast::visitor_info::event_type::container_entity_exit:
        // Search inside any (nested) types defined in this namespaces.
        return true;
      case cppast::visitor_info::event_type::leaf_entity:
          break;
      default:
          throw std::logic_error("visitor in unexpected state");
    }

    if (entity.kind() == cppast::cpp_entity_kind::friend_t) {
      const auto declaration = static_cast<const cppast::cpp_friend&>(entity).entity();
      if (declaration.has_value() && matches(declaration.value(), name)) {
        frend = type_safe::ref(declaration.value());
        // Abort the search.
        return false;
      }
    }

    // Continue the search.
    return true;
  });

  return frend;
}

type_safe::optional_ref<const cppast::cpp_entity> symbols::impl::cppast_symbols::base_class(const cppast::cpp_entity& root_, const std::string& name_) const {
  std::string name = name_;

  boost::erase_all(name, " ");

  type_safe::object_ref<const cppast::cpp_entity> root{root_};

  if (cppast::is_template(root->kind()))
    root = type_safe::ref(*static_cast<const cppast::cpp_template&>(*root).begin());

  if (root->kind() != cppast::cpp_class::kind())
    return type_safe::nullopt;

  for (const auto& base : static_cast<const cppast::cpp_class&>(*root).bases())
    if (base.name() == name)
      return type_safe::ref(base);

  return type_safe::nullopt;
}

type_safe::optional_ref<const cppast::cpp_entity> symbols::impl::cppast_symbols::template_parameter(const cppast::cpp_entity& root, const std::string& name) const {
  if (cppast::is_template(root.kind()))
    for (const auto& param : static_cast<const cppast::cpp_template&>(root).parameters())
      if (param.name() == name)
        return type_safe::ref(param);

  return type_safe::nullopt;
}

bool symbols::impl::cppast_symbols::matches(const cppast::cpp_entity& entity, const std::string& search) const {
  if (entity.kind() == cppast::cpp_file::kind())
    // We do not want to match with C++ header files here. They are handled separately.
    return false;

  if (entity.kind() == cppast::cpp_friend::kind())
    // We do not want to match with friend declarations but only with what they
    // are declaring (in their scope.)
    return false;

  const auto name = boost::erase_all_copy(entity.name(), " ");
  const auto temp = template_parameters(entity);
  const auto sig = boost::erase_all_copy(signature(entity), " ");
  const auto names = parameter_names(entity);

  if (search == name) return true;
  if (search == name + temp) return true;
  if (search == name + sig) return true;
  if (search == name + names) return true;
  if (search == name + temp + sig) return true;
  if (search == name + temp + names) return true;

  return false;
}

std::string symbols::impl::cppast_symbols::template_parameters(const cppast::cpp_entity& entity) const {
  if (!cppast::is_template(entity.kind()))
      return "";

  const auto& templ = static_cast<const cppast::cpp_template&>(entity);

  std::string result = "<";
  for (auto& param : templ.parameters())
      result += param.name() + ",";
  result.back() = '>';

  return result;
}

std::string symbols::impl::cppast_symbols::parameter_names(const cppast::cpp_entity& entity) const {
  if (cppast::is_template(entity.kind()))
    return parameter_names(*static_cast<const cppast::cpp_template&>(entity).begin());
  if (!cppast::is_function(entity.kind()))
    return "";

  std::string names;

  for (const auto& param : static_cast<const cppast::cpp_function_base&>(entity).parameters()) {
    if (param.name().empty())
      return "";
    names += param.name();
    names += ",";
  }

  return "(" + names.substr(0, names.size() - 1) + ")";
}

std::string symbols::impl::cppast_symbols::signature(const cppast::cpp_entity& entity) const {
  if (cppast::is_template(entity.kind()))
    return signature(*static_cast<const cppast::cpp_template&>(entity).begin());
  if (!cppast::is_function(entity.kind()))
    return "";
  return static_cast<const cppast::cpp_function_base&>(entity).signature();
}

symbols::impl::sphinx_symbols::sphinx_symbols(const sphinx::documentation_set* inventory) : inventory(inventory) {}

std::optional<model::link_target> symbols::impl::sphinx_symbols::find(const std::string& name) const {
  std::optional<sphinx::entry> match;

  // TODO(0.6.0-beta): We should be much more fuzzy here.
  // TODO(0.6.0-rc): We could be better than O(n) here.
  for (const auto& entry : inventory->entries) {
    if (entry.name == name) {
      if (!match.has_value() || match.value().priority > entry.priority)
        match = entry;
    }
  }

  if (!match.has_value())
    return std::nullopt;

  return model::link_target::sphinx_target(*inventory, match.value());
}

symbols::impl::unique_name_symbols::unique_name_symbols(const unique_name_inventory* inventory): cppast_symbols(inventory->inventory) {
  for (const auto& entity : *inventory->entities) {
    model::visitor::visit([&](auto&& entity, auto&& recurse) {
      using T = std::decay_t<decltype(entity)>;
      if constexpr (std::is_base_of_v<model::cpp_entity_documentation, T>) {
        if (!entity.id.empty()) {
          if (uniquely_named.find(entity.id) != uniquely_named.end())
            logger::warn(fmt::format("Found more than one entity with id/unique_name {}. Ignoring {} in symbol lookup table.", entity.id, entity.entity().name()));
          else
            uniquely_named[entity.id] = &entity.entity();
        }
      }
      recurse();
    }, entity);
  }
}

std::optional<model::link_target> symbols::impl::unique_name_symbols::find(const std::string& name) const {
  const auto find = uniquely_named.find(name);
  if (find != uniquely_named.end())
    return model::link_target{&*find->second};

  const auto split = impl::split(name);
  if (split) {
    const auto [prefix, suffix] = *split;
    const auto find = uniquely_named.find(prefix);
    if (find != uniquely_named.end()) {
      auto descendant = cppast_symbols.descendant(*find->second, suffix);
      if (descendant)
        return model::link_target{&descendant.value()};
    }
  }

  return std::nullopt;
}

}
