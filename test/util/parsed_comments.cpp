// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <cppast/visitor.hpp>

#include "parsed_comments.hpp"
#include "unindent.hpp"

#include "../../standardese/inventory/cppast_inventory.hpp"
#include "../../standardese/parser/comment_parser.hpp"
#include "../../standardese/model/cpp_entity_documentation.hpp"
#include "../../standardese/transformer/create_uncommented_child_transformer.hpp"

namespace standardese::test::util {

parsed_comments::parsed_comments(const cpp_file& header): header(header) {
  cppast::visit(header, [&](const auto& entity, const auto& info) {
    if (info.is_old_entity())
        // Continue visit but do not register this container twice.
        return true;

    if (cppast::is_templated(entity))
        // Ignore templates themselves since we will only handle what's inside the template.
        return true;

    auto documentation = model::cpp_entity_documentation{&entity, header};
    documentation.exclude_mode = model::exclude_mode::uncommented;
    entity_set_insert(entities, documentation);

    return true;
  });

  entity_set_extend(entities, transformer::create_uncommented_child_transformer{&entities, header}.transform());
}

parsed_comments&& parsed_comments::add(const cppast::cpp_entity& target, const std::string& comment, parser::comment_parser::comment_parser_options options) && {
  const auto resolve = [&](const std::string& name) -> type_safe::optional_ref<const cppast::cpp_entity> {
      return inventory::cppast_inventory::find(name, target, header);
  };

  auto parser = parser::comment_parser(options, header);

  auto parsed = parser.parse(util::unindent(comment), target, resolve);
  for (auto& entity : parsed) {
    if (entity.is<model::cpp_entity_documentation>()) {
      const auto& documentation = entity.as<model::cpp_entity_documentation>();
      auto existing = entity_set_find(entities, documentation.entity());
      if (existing != entities.end() && existing->as<model::cpp_entity_documentation>().exclude_mode == model::exclude_mode::uncommented)
        entities.erase(existing);
    }

    entity_set_insert(entities, entity);
  }

  const auto* file = &target;
  while (file->parent()) file = &file->parent().value();
  assert(file->kind() == cppast::cpp_file::kind());

  return std::move(*this);
}

model::cpp_entity_documentation parsed_comments::as_documentation() const {
  auto eligible = [](const auto& entity) {
    if (!entity.template is<model::cpp_entity_documentation>())
      return false;
    if (entity.template as<const model::cpp_entity_documentation>().exclude_mode == model::exclude_mode::uncommented)
      return false;

    return true;
  };

  for (auto it = entities.begin(); it != entities.end(); ++it) {
    if (eligible(*it)) {
      auto ret = it->as<const model::cpp_entity_documentation>();

      for (++it; it != entities.end(); ++it)
        if (eligible(*it))
          throw std::logic_error("Cannot convert parsed_comments to cpp_entity_documentation. There is no unique such entity.");

      return ret;
    }
  }

  throw std::logic_error("No cpp_entity_documentation has been parsed.");
}

model::cpp_entity_documentation parsed_comments::as_documentation(const std::string& name) const {
  return this->operator[](name).as<model::cpp_entity_documentation>();
}

model::cpp_entity_documentation parsed_comments::as_documentation(const cppast::cpp_entity& entity) const {
  return this->operator[](entity).as<model::cpp_entity_documentation>();
}

model::module parsed_comments::as_module() const {
  auto eligible = [](const auto& entity) {
    if (!entity.template is<model::module>())
      return false;
    if (entity.template as<const model::module>().exclude_mode == model::exclude_mode::uncommented)
      return false;

    return true;
  };

  for (auto it = entities.begin(); it != entities.end(); ++it) {
    if (eligible(*it)) {
      auto ret = it->as<const model::module>();

      for (++it; it != entities.end(); ++it)
        if (eligible(*it))
          throw std::logic_error("Cannot convert parsed_comments to module. There is no unique such entity.");

      return ret;
    }
  }

  throw std::logic_error("No module has been parsed.");
}

parsed_comments::operator model::entity() const {
  auto eligible = [](const auto& entity) {
    if (entity.template is<model::mixin::documentation>())
      if (entity.template as<model::mixin::documentation>().exclude_mode == model::exclude_mode::uncommented)
      return false;

    return true;
  };

  for (auto it = entities.begin(); it != entities.end(); ++it) {
    if (eligible(*it)) {
      auto ret = *it;

      for (++it; it != entities.end(); ++it)
        if (eligible(*it))
          throw std::logic_error("Cannot convert parsed_comments to module. There is no unique such entity.");

      return ret;
    }
  }

  throw std::logic_error("Nothing has been parsed yet.");
}

model::entity parsed_comments::operator[](type_safe::object_ref<const cppast::cpp_entity> target) const {
  return entity_set_at(entities, *target);
}

model::entity parsed_comments::operator[](const std::string& target) const {
  return (*this)[type_safe::ref(header[target])];
}

}
