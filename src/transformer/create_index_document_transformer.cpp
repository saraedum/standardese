// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <fmt/format.h>
#include <cppast/cpp_file.hpp>

#include "../../standardese/transformer/create_index_document_transformer.hpp"
#include "../../standardese/model/document.hpp"
#include "../../standardese/model/markup/link.hpp"
#include "../../standardese/model/markup/list.hpp"
#include "../../standardese/model/link_target.hpp"
#include "../../standardese/model/visitor/visit.hpp"
#include "../../standardese/model/cpp_entity_documentation.hpp"
#include "../../standardese/model/markup/list_item.hpp"
#include "../../standardese/model/unordered_entities.hpp"
#include "../../standardese/model/module.hpp"
#include "../../standardese/logger.hpp"
#include "../../standardese/output_generator/xml/xml_generator.hpp"

// TODO(0.6.0-alpha): This is not producing the same output as standardese <0.6.0 used to produce yet.

namespace standardese::transformer
{

create_index_document_transformer::create_index_document_transformer_options::create_index_document_transformer_options() : predicate([](const model::entity&) { return true; }) {}

create_index_document_transformer::create_index_document_transformer(model::unordered_entities& entities, parser::cpp_context context, create_index_document_transformer_options options) : entities(entities), options(options), anchor_text_formatter(options.anchor_text_options, std::move(context)) {}

model::document create_index_document_transformer::transform(threading::pool::factory workers) const {
  auto list = model::markup::list(false);

  for (auto& entity : entities)
    if (options.predicate(entity))
      model::visitor::visit([&](auto&& documentation) {
        using T = std::decay_t<decltype(documentation)>;

        if constexpr (std::is_same_v<T, model::cpp_entity_documentation> || std::is_same_v<T, model::module>) {
          model::link_target target("");
          if constexpr (std::is_same_v<T, model::cpp_entity_documentation>) {
            target = model::link_target(documentation.entity());
          } else {
            target = model::link_target::module_target(documentation.name);
          }

          // We create a link with a target but no "text". A transformer
          // such as the anchor_text_transformer will fill in that text.
          auto link = model::markup::link(target, "");

          list.children.push_back(model::markup::list_item({link}));
        } else {
          throw std::logic_error("unexpected entity in index document builder");
        }
      }, entity);

  return model::document(options.name, options.path, {std::move(list)});
}

bool create_index_document_transformer::is_header_file(const model::entity& entity) {
  return model::visitor::visit([](auto&& entity) {
    using T = std::decay_t<decltype(entity)>;
    if constexpr (std::is_same_v<T, model::cpp_entity_documentation>) {
      return entity.entity().kind() == cppast::cpp_file::kind();
    }
    return false;
  }, entity);
}

bool create_index_document_transformer::is_module(const model::entity& entity) {
  return model::visitor::visit([](auto&& entity) {
    using T = std::decay_t<decltype(entity)>;
    return std::is_same_v<T, model::module>;
  }, entity);
}

}
