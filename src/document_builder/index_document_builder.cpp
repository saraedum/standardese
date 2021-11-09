// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <cppast/cpp_file.hpp>

#include "../../standardese/document_builder/index_document_builder.hpp"
#include "../../standardese/model/document.hpp"
#include "../../standardese/model/markup/link.hpp"
#include "../../standardese/model/markup/list.hpp"
#include "../../standardese/model/link_target.hpp"
#include "../../standardese/model/visitor/visit.hpp"
#include "../../standardese/model/cpp_entity_documentation.hpp"
#include "../../standardese/model/markup/list_item.hpp"
#include "../../standardese/model/unordered_entities.hpp"
#include "../../standardese/model/module.hpp"

using standardese::model::document;

namespace standardese::document_builder
{

document index_document_builder::operator()(const std::function<bool(const model::entity&)> predicate, const model::unordered_entities& entities) const {
  auto list = model::markup::list(false);
  
  for (auto& entity : entities)
    if (predicate(entity))
      model::visitor::visit(entity, [&](const model::cpp_entity_documentation& documentation) {
          list.add_child(model::markup::list_item(model::markup::link(documentation.entity(), "", documentation.entity().name())));
      }, [&](const model::module& module) {
          list.add_child(model::markup::list_item(model::markup::link(model::link_target::module_target(module.name), "", module.name)));
      });

  // TODO
  return document("TODO", std::move(list));
}

bool index_document_builder::is_header_file(const model::entity& entity) {
  return model::visitor::visit(entity, [](const model::cpp_entity_documentation& entity) {
      return entity.entity().kind() == cppast::cpp_file::kind();
  }, []() {
      return false;
  });
}

bool index_document_builder::is_module(const model::entity& entity) {
  return model::visitor::visit(entity, [](const model::module&) {
      return true;
  }, []() {
      return false;
  });
}

}
