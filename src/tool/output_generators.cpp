// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <stdexcept>
#include <fstream>

#include "../../standardese/tool/output_generators.hpp"
#include "../../standardese/model/unordered_entities.hpp"
#include "../../standardese/model/document.hpp"
#include "../../standardese/output_generator/markdown/markdown_generator.hpp"
#include "../../standardese/output_generator/xml/xml_generator.hpp"
#include "../../standardese/output_generator/sphinx/inventory_generator.hpp"
#include "../../standardese/logger.hpp"

namespace standardese::tool {

output_generators::output_generators(struct options options) : options(options) {}

void output_generators::emit(model::unordered_entities& documents) {
  // TODO: Make this configurable.
  /*
  for (auto& document : documents) {
    std::ofstream out("TODO.xml");
    auto generator = output_generator::xml::xml_generator{out};
    document.accept(generator);
  }
  */
  for (auto& document : documents) {
    std::ofstream out(options.output_directory / (document.as<model::document>().name + ".md"));
    auto generator = output_generator::markdown::markdown_generator{out};
    document.accept(generator);
  }

  {
    std::ofstream out(options.output_directory / "objects.inv");
    auto generator = output_generator::sphinx::inventory_generator{out};
    for (auto& document : documents) {
      document.accept(generator);
    }
  }
}

}
