// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <fmt/format.h>
#include <cppast/cpp_file.hpp>
#include <nlohmann/json.hpp>

#include "../../standardese/tool/document_builders.hpp"
#include "../../standardese/model/unordered_entities.hpp"
#include "../../standardese/model/cpp_entity_documentation.hpp"
#include "../../standardese/document_builder/entity_document_builder.hpp"
#include "../../standardese/model/document.hpp"
#include "../../standardese/formatter/inja_formatter.hpp"
#include "../../standardese/logger.hpp"

namespace standardese::tool {

document_builders::document_builders(struct options options) : options(std::move(options)) {}

model::unordered_entities document_builders::create(model::unordered_entities& parsed, parser::cpp_context context) {
  // TODO: Make configurable. We presently only build for header files in fixed formats.

  auto builder = document_builder::entity_document_builder();

  model::unordered_entities documents;

  formatter::inja_formatter formatter{{}, context};

  logger::info("Creating entity documents.");
  for (auto& entity : parsed) {
    if (entity.is<model::cpp_entity_documentation>()) {
      auto& documentation = entity.as<model::cpp_entity_documentation>();

      formatter::inja_formatter inja{{}, context};
      inja.data().merge_patch(inja.to_json(documentation.entity()));

      if (cppast::cpp_file::kind() == documentation.entity().kind()) {
        logger::debug(fmt::format("Creating document for entity {}.", documentation.entity().name()));

        const std::string name = inja.format(options.document_name);
        const std::string path = inja.format(options.document_path);

        documents.insert(builder.build(name, path, entity, parsed));
      }
    }
    if (entity.is<model::document>()) {
      documents.insert(entity);
    }
  }

  // TODO: Build index files.

  // TODO: Make sure document names/paths are unique.

  return documents;
}

}
