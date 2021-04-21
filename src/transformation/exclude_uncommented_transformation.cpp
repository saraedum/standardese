// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <stack>

#include <fmt/format.h>

#include "../../standardese/transformation/exclude_uncommented_transformation.hpp"
#include "../../standardese/model/visitor/recursive_visitor.hpp"
#include "../../standardese/model/visitor/generic_visitor.hpp"
#include "../../standardese/model/entity.hpp"
#include "../../standardese/model/cpp_entity_documentation.hpp"
#include "../../standardese/logger.hpp"

namespace standardese::transformation {

namespace {

/// Removes documentation from an entity tree that should be excluded.
struct visitor : model::visitor::generic_visitor<visitor, model::visitor::recursive_visitor<false>> {
  visitor(const struct exclude_uncommented_transformation::options& options);

  exclude_uncommented_transformation::options::mode mode(const cppast::cpp_entity& entity) const;

  enum class exclusion {
    exclude,
    skip,
    include,
  };

  exclusion exclude(exclude_uncommented_transformation::options::mode mode, bool empty, model::exclude_mode uncommented) const;

  /// Push true to `empty`.
  void push();

  /// Pop from `empty`.
  void pop();

  template <typename T>
  void operator()(T& entity);

  std::stack<bool> empty;
  std::stack<std::vector<model::entity>> containers;

  const struct exclude_uncommented_transformation::options& options;
};

}

exclude_uncommented_transformation::exclude_uncommented_transformation(model::unordered_entities& documents, struct options options) : transformation(documents), options(std::move(options)) {}

void exclude_uncommented_transformation::do_transform(model::entity& document) {
  visitor v{options};
  document.accept(v);
}

namespace {

visitor::visitor(const struct exclude_uncommented_transformation::options& options) : options(options) {
  containers.push({});
  push();
}

exclude_uncommented_transformation::options::mode visitor::mode(const cppast::cpp_entity& entity) const {
  switch(entity.kind()) {
    case cppast::cpp_entity_kind::access_specifier_t:
    case cppast::cpp_entity_kind::base_class_t:
    case cppast::cpp_entity_kind::function_parameter_t:
    case cppast::cpp_entity_kind::non_type_template_parameter_t:
    case cppast::cpp_entity_kind::template_template_parameter_t:
    case cppast::cpp_entity_kind::template_type_parameter_t:
    case cppast::cpp_entity_kind::include_directive_t:
    case cppast::cpp_entity_kind::language_linkage_t:
    case cppast::cpp_entity_kind::namespace_alias_t:
    case cppast::cpp_entity_kind::macro_parameter_t:
    case cppast::cpp_entity_kind::static_assert_t:
    case cppast::cpp_entity_kind::unexposed_t:
    case cppast::cpp_entity_kind::using_directive_t:
    case cppast::cpp_entity_kind::using_declaration_t:
      return exclude_uncommented_transformation::options::mode::exclude_if_empty;
    case cppast::cpp_entity_kind::alias_template_t:
    case cppast::cpp_entity_kind::type_alias_t:
      return options.exclude_alias;
    case cppast::cpp_entity_kind::class_template_specialization_t:
    case cppast::cpp_entity_kind::class_t:
    case cppast::cpp_entity_kind::class_template_t:
    case cppast::cpp_entity_kind::enum_t:
      return options.exclude_class;
    case cppast::cpp_entity_kind::bitfield_t:
    case cppast::cpp_entity_kind::member_variable_t:
    case cppast::cpp_entity_kind::variable_template_t:
    case cppast::cpp_entity_kind::enum_value_t:
    case cppast::cpp_entity_kind::variable_t:
      return options.exclude_variable;
    case cppast::cpp_entity_kind::member_function_t:
    case cppast::cpp_entity_kind::function_t:
    case cppast::cpp_entity_kind::function_template_specialization_t:
    case cppast::cpp_entity_kind::function_template_t:
    case cppast::cpp_entity_kind::constructor_t:
    case cppast::cpp_entity_kind::conversion_op_t:
    case cppast::cpp_entity_kind::destructor_t:
      return options.exclude_function;
    case cppast::cpp_entity_kind::file_t:
      return options.exclude_file;
    case cppast::cpp_entity_kind::friend_t:
      return options.exclude_friend;
    case cppast::cpp_entity_kind::macro_definition_t:
      return options.exclude_macro;
    case cppast::cpp_entity_kind::namespace_t:
      return options.exclude_namespace;
    default:
      logger::error(fmt::format("Ignoring unexpected C++ entity `{}` if uncommented.", entity.name()));
      return exclude_uncommented_transformation::options::mode::exclude;
  }
}

visitor::exclusion visitor::exclude(exclude_uncommented_transformation::options::mode mode, bool empty, model::exclude_mode uncommented) const {
  switch(uncommented) {
    case model::exclude_mode::exclude:
      return exclusion::exclude;
    case model::exclude_mode::uncommented:
      break;
    default:
      return exclusion::include;
  }

  switch(mode) {
    case exclude_uncommented_transformation::options::mode::exclude:
      return exclusion::exclude;
    case exclude_uncommented_transformation::options::mode::exclude_if_empty:
      if (empty) return exclusion::exclude;
      else return exclusion::include;
    case exclude_uncommented_transformation::options::mode::skip:
      return exclusion::skip;
    case exclude_uncommented_transformation::options::mode::include:
      return exclusion::include;;
  }
}

void visitor::push() {
  empty.push(true);
}

void visitor::pop() {
  bool top = empty.top();

  empty.pop();

  if (!top)
    empty.top() = false;
}

template <typename T>
void visitor::operator()(T& entity) {
  static_assert(std::is_same_v<T, std::decay_t<T>>);

  push();

  containers.push({});
  model::visitor::recursive_visitor<false>::visit(entity);

  if constexpr (std::is_base_of_v<model::mixin::container<>, T>) {
    entity.clear();
    for (auto& child : containers.top())
      entity.emplace_child(std::move(child));
  }
  containers.pop();

  if constexpr (std::is_same_v<T, model::cpp_entity_documentation>) {
    switch (exclude(mode(entity.entity()), empty.top(), entity.exclude_mode)) {
      case exclusion::exclude:
        break;
      case exclusion::include:
        empty.top() = false;
        containers.top().emplace_back(std::move(entity));
        break;
      case exclusion::skip:
        empty.top() = false;
        for (auto& child: entity)
          containers.top().emplace_back(std::move(child));
        break;
    }
  } else {
    containers.top().emplace_back(entity);
  }

  if constexpr (std::is_same_v<T, model::markup::block_quote>
    || std::is_same_v<T, model::markup::code_block>
    || std::is_same_v<T, model::markup::code>
    || std::is_same_v<T, model::markup::emphasis>
    || std::is_same_v<T, model::markup::hard_break>
    || std::is_same_v<T, model::markup::heading>
    || std::is_same_v<T, model::markup::image>
    || std::is_same_v<T, model::markup::link>
    || std::is_same_v<T, model::markup::list>
    || std::is_same_v<T, model::markup::list_item>
    || std::is_same_v<T, model::markup::paragraph>
    || std::is_same_v<T, model::markup::soft_break>
    || std::is_same_v<T, model::markup::strong_emphasis>
    || std::is_same_v<T, model::markup::text>
    || std::is_same_v<T, model::markup::thematic_break>) {
    empty.top() = false;
  }

  pop();
}

}

}
