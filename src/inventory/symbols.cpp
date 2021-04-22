// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <regex>

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
#include <cppast/visitor.hpp>

#include "../../standardese/inventory/symbols.hpp"
#include "../../standardese/inventory/cppast_inventory.hpp"
#include "../../standardese/inventory/sphinx/documentation_set.hpp"
#include "../../standardese/logger.hpp"

// TODO: We do not handle friend declarations correctly here. A class can
// declare a friend function that then lives in the surrounding namespace of
// the class. (It seems that friend namespace::f() is not possible, or rather
// only possible if f() has been forward declared anyway.) The old standardese
// had some handling for this.

namespace standardese::inventory {

class symbols::impl {
 public:
  virtual ~impl() {}

  virtual type_safe::optional<model::link_target> find(const std::string& name) const = 0;
  virtual type_safe::optional<model::link_target> find(const std::string& name, const cppast::cpp_entity& entity) const;

  template <typename T>
  class generic_symbols;

  class cppast_symbols;
  class doxygen_symbols;
  class sphinx_symbols;
};

template <typename T>
class symbols::impl::generic_symbols : public symbols::impl {
 public:
  virtual type_safe::optional_ref<const T> descendant(const T&, const std::string& name) const;
  virtual type_safe::optional_ref<const T> child(const T&, const std::string& name) const = 0;
  virtual type_safe::optional_ref<const T> parameter(const T&, const std::string& name) const = 0;
  virtual type_safe::optional_ref<const T> template_parameter(const T&, const std::string& name) const = 0;
  virtual type_safe::optional_ref<const T> base_class(const T&, const std::string& name) const = 0;
};

class symbols::impl::cppast_symbols : public symbols::impl::generic_symbols<cppast::cpp_entity> {
 public:
  cppast_symbols(const cppast_inventory&);

  type_safe::optional<model::link_target> find(const std::string& name) const override;
  type_safe::optional<model::link_target> find(const std::string& name, const cppast::cpp_entity& entity) const override;

  type_safe::optional_ref<const cppast::cpp_entity> child(const cppast::cpp_entity&, const std::string& name) const override;
  type_safe::optional_ref<const cppast::cpp_entity> parameter(const cppast::cpp_entity&, const std::string& name) const override;
  type_safe::optional_ref<const cppast::cpp_entity> template_parameter(const cppast::cpp_entity&, const std::string& name) const override;
  type_safe::optional_ref<const cppast::cpp_entity> base_class(const cppast::cpp_entity&, const std::string& name) const override;

 private:
  bool matches(const cppast::cpp_entity& entity, const std::string& search) const;
  std::string template_parameters(const cppast::cpp_entity& entity) const;
  std::string parameter_names(const cppast::cpp_entity& entity) const;
  std::string signature(const cppast::cpp_entity& entity) const;

  const cppast_inventory& inventory;
};

class symbols::impl::sphinx_symbols : public symbols::impl {
 public:
  sphinx_symbols(const sphinx::documentation_set&);

  type_safe::optional<model::link_target> find(const std::string& name) const override;

 private:
  const sphinx::documentation_set& inventory;
};

symbols::symbols(const inventory& inventory) {
  if (dynamic_cast<const cppast_inventory*>(&inventory) != nullptr) {
    self = std::make_unique<impl::cppast_symbols>(static_cast<const cppast_inventory&>(inventory));
  } else if (dynamic_cast<const sphinx::documentation_set*>(&inventory) != nullptr) {
    self = std::make_unique<impl::sphinx_symbols>(static_cast<const sphinx::documentation_set&>(inventory));
  } else {
    throw std::logic_error("not implemented: symbols for this type of inventory");
  }
}

symbols::symbols(symbols&& value) : self(std::move(value.self)) {}

symbols::~symbols() {}

type_safe::optional<model::link_target> symbols::find(const std::string& name_) const {
  std::string name = name_; 

  if (boost::starts_with(name, "::"))
    // A name in the global scope. But it does not matter since we are already in the global scope.
    name = name.substr(2);

  if (name.empty())
    return type_safe::nullopt;

  return self->find(name);
}

type_safe::optional<model::link_target> symbols::find(const std::string& name, const cppast::cpp_entity& entity) const {
  if (entity.kind() == cppast::cpp_file::kind())
    return this->find(name);

  if (boost::starts_with(name, "::"))
    // A name in the global scope. Do not try to look it up relative to entity.
    return this->find(name);

  if (name.empty())
    return type_safe::nullopt;

  return self->find(name, entity);
}

type_safe::optional<model::link_target> symbols::impl::find(const std::string& name, const cppast::cpp_entity&) const {
  return find(name);
}

symbols::impl::cppast_symbols::cppast_symbols(const cppast_inventory& inventory) : inventory(inventory) {}

type_safe::optional<model::link_target> symbols::impl::cppast_symbols::find(const std::string& name) const {
  for (auto* root : inventory.roots) {
      auto search = descendant(*root, name);
      if (search)
          return search.value();
  }
      
  return type_safe::nullopt;
}

type_safe::optional<model::link_target> symbols::impl::cppast_symbols::find(const std::string& name, const cppast::cpp_entity& entity) const {
  if (inventory.roots.find(&cppast_inventory::root(entity)) == inventory.roots.end())
    throw std::invalid_argument("Cannot look up symbol relative to something not defined in any of the loaded files.");

  auto search = descendant(entity, name);
  if (search.has_value())
      return search.value();

  if (!entity.parent().has_value())
    return this->find(name);

  return this->find(name, entity.parent().value());
}

template <typename T>
type_safe::optional_ref<const T> symbols::impl::generic_symbols<T>::descendant(const T& root, const std::string& name) const {
  if (name.empty())
      return type_safe::nullopt;

  // We do not distinguish these two operators at all `.` and `::`. Originally,
  // we used `.` for function arguments and `::` in the usual C++ sense but
  // there seems to be not much of a point in enforcing such rules.
  static auto separator = std::regex(R"(\.|::)");

  // The recursive case: if the name contains `::` or `.`, split the name at
  // this separator and search recursively.
  std::smatch match;
  if (std::regex_search(name, match, separator)) {
      auto child = descendant(root, match.prefix());
      if (child)
          return descendant(child.value(), match.suffix());

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

    if (matches(entity, name))
        child = type_safe::ref(entity);

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
          if (child)
              // Abort the search.
              return false;
          // Continue the search.
          return true;
      default:
          throw std::logic_error("visitor in unexpected state");
    }

  });

  // It is customary to write `typedef struct S {} S;` or `typedef struct {}
  // S;` especially in C. Technically, an "S" would refer to the typedef, but
  // this is never what people want, instead we refer to the struct.
  // TODO: How should we do this exactly? Should we run this until we are at a non-trivial typedef?
  if (child && child.value().kind() == cppast::cpp_type_alias::kind()) {
    const auto& alias = static_cast<const cppast::cpp_type_alias&>(child.value());
    const auto& type = alias.underlying_type();
    if (type.kind() == cppast::cpp_type_kind::user_defined_t) {
      child = inventory.context.index().lookup(*static_cast<const cppast::cpp_user_defined_type&>(type).entity().id().begin());
    }
  }

  return child;
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
      logger::error("not implemented: cannot match friends yet");
  
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

symbols::impl::sphinx_symbols::sphinx_symbols(const sphinx::documentation_set& inventory) : inventory(inventory) {}

type_safe::optional<model::link_target> symbols::impl::sphinx_symbols::find(const std::string& name) const {
  type_safe::optional<sphinx::entry> match;

  // TODO: We should be much more fuzzy here.
  // TODO: We could be better than O(n) here.
  for (const auto& entry : inventory.entries) {
    if (entry.name == name) {
      if (!match.has_value() || match.value().priority > entry.priority)
        match = entry;
    }
  }

  if (!match.has_value())
    return type_safe::nullopt;

  return model::link_target::sphinx_target(inventory, match.value());
}

}
