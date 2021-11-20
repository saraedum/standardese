// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <boost/filesystem/operations.hpp>
#include <fmt/format.h>
#include <stdexcept>
#include <fstream>

#include "../../standardese/tool/output_generators.hpp"
#include "../../standardese/model/entity_set.hpp"
#include "../../standardese/model/document.hpp"
#include "../../standardese/output_generator/markdown/markdown_generator.hpp"
#include "../../standardese/output_generator/xml/xml_generator.hpp"
#include "../../standardese/output_generator/sphinx/inventory_generator.hpp"
#include "../../standardese/output_generator/doxygen/tagfile_generator.hpp"
#include "../../standardese/logger.hpp"

namespace standardese::tool {

output_generators::output_generators(output_generators_options options) : options(options) {}

void output_generators::emit(model::entity_set& documents) {
  logger::info("Rendering output documents.");

  const auto open = [&](const boost::filesystem::path& path) {
    logger::info(fmt::format("Writing {}.", path.native()));
    boost::filesystem::create_directories(path.parent_path());
    return std::ofstream{path};
  };

  // TODO(0.6.0-beta): Be error tolerant everywhere. Catch and report errors in every step.

  // TODO(0.6.0-beta): Write outputs in parallel.

  if (options.primary_format == output_generators_options::output_format::markdown) {
    for (auto& document : documents) {
      auto out = open(options.output_directory / (document.as<model::document>().name + ".md"));
      auto generator = output_generator::markdown::markdown_generator{&out, options.markdown_options};
      document.accept(generator);
    }
  } else if (options.primary_format == output_generators_options::output_format::xml) {
    for (auto& document : documents) {
      auto out = open(options.output_directory / (document.as<model::document>().name + ".xml"));
      auto generator = output_generator::xml::xml_generator{&out};
      document.accept(generator);
    }
  }

  // TODO(0.6.0-alpha): HTML output.

  // TODO(0.6.0-alpha): Text output.

  if (!options.intersphinx_inventory.empty()) {
    auto out = open(options.output_directory / options.intersphinx_inventory);
    auto generator = output_generator::sphinx::inventory_generator{&out};
    for (auto& document : documents) {
      document.accept(generator);
    }
  }

  if (!options.doxygen_tagfile.empty()) {
    auto out = open(options.output_directory / options.doxygen_tagfile);
    auto generator = output_generator::doxygen::tagfile_generator{&out};
    for (auto& document : documents) {
      document.accept(generator);
    }
  }
}

}
