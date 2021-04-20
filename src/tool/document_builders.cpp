// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <cppast/cpp_file.hpp>

#include "../../standardese/tool/document_builders.hpp"
#include "../../standardese/model/unordered_entities.hpp"
#include "../../standardese/model/cpp_entity_documentation.hpp"
#include "../../standardese/document_builder/entity_document_builder.hpp"
#include "../../standardese/model/document.hpp"
#include "../../standardese/logger.hpp"

namespace standardese::tool {

document_builders::document_builders(struct options) {}

model::unordered_entities document_builders::create(model::unordered_entities& parsed) {
  // TODO: Make configurable. We presently only build for header files in fixed formats.

  auto builder = document_builder::entity_document_builder();

  model::unordered_entities documents;

  for (const auto& entity : parsed) {
    if (entity.is<model::cpp_entity_documentation>()) {
      auto& documentation = entity.as<model::cpp_entity_documentation>();
      if (cppast::cpp_file::kind() == documentation.entity().kind()) {
        documents.insert(builder.build(entity, parsed));
      }
    }
    if (entity.is<model::document>()) {
      documents.insert(entity);
    }
  }

  // TODO: Make sure document names/paths are unique.
  // TODO: Set document paths.

  return documents;
}

}
