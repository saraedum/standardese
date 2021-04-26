// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include "parsed_comments.hpp"
#include "unindent.hpp"

#include "../../standardese/inventory/cppast_inventory.hpp"
#include "../../standardese/parser/comment_parser.hpp"
#include "../../standardese/model/cpp_entity_documentation.hpp"

namespace standardese::test::util {

parsed_comments::parsed_comments(const cpp_file& header) : header(header) {}

parsed_comments&& parsed_comments::add(const cppast::cpp_entity& target, const std::string& comment, parser::comment_parser::comment_parser_options options) && {
  const auto resolve = [&](const std::string& name) -> type_safe::optional_ref<const cppast::cpp_entity> {
      return inventory::cppast_inventory::find(name, target, header);
  };

  auto parser = parser::comment_parser(options, header);

  if (entities.begin() != entities.end())
    throw std::logic_error("not implemented: merge entities");

  auto parsed = parser.parse(util::unindent(comment), target, resolve);
  entities = model::unordered_entities(parsed);

  const auto* file = &target;
  while (file->parent()) file = &file->parent().value();
  assert(file->kind() == cppast::cpp_file::kind());

  parser.add_uncommented_entities(entities, static_cast<const cppast::cpp_file&>(*file));
  parser.add_uncommented_modules(entities);

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
  return entities.cpp_entity(*target);
}

model::entity parsed_comments::operator[](const std::string& target) const {
  return (*this)[type_safe::ref(header[target])];
}

}
