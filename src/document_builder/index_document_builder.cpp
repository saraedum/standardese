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

index_document_builder::options::options() {}

index_document_builder::index_document_builder(options options) : anchor_text_formatter(options.anchor_text_options) {}

document index_document_builder::build(const std::string& name, const std::function<bool(const model::entity&)> predicate, const model::unordered_entities& entities) const {
  auto list = model::markup::list(false);
  
  for (auto& entity : entities)
    if (predicate(entity))
      model::visitor::visit([&](auto&& documentation) {
        using T = std::decay_t<decltype(documentation)>;
        if constexpr (std::is_same_v<T, model::cpp_entity_documentation>) {
          list.add_child(model::markup::list_item(model::markup::link(documentation.entity(), "", anchor_text_formatter.format(documentation.entity()))));
        } else if constexpr (std::is_same_v<T, model::module>) {
          list.add_child(model::markup::list_item(model::markup::link(model::link_target::module_target(documentation.name), "", anchor_text_formatter.format(documentation))));
        } else {
          throw std::logic_error("unexpected entity in index document builder");
        }
      }, entity);

  // TODO: Set name & path properly.
  return document(name, name, std::move(list));
}

bool index_document_builder::is_header_file(const model::entity& entity) {
  return model::visitor::visit([](auto&& entity) {
    using T = std::decay_t<decltype(entity)>;
    if constexpr (std::is_same_v<T, model::cpp_entity_documentation>) {
      return entity.entity().kind() == cppast::cpp_file::kind();
    }
    return false;
  }, entity);
}

bool index_document_builder::is_module(const model::entity& entity) {
  return model::visitor::visit([](auto&& entity) {
    using T = std::decay_t<decltype(entity)>;
    return std::is_same_v<T, model::module>;
  }, entity);
}

}
