// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <fmt/format.h>

#include "../../standardese/model/document.hpp"
#include "../../standardese/model/markup/paragraph.hpp"
#include "../../standardese/logger.hpp"
#include "../../standardese/output_generator/xml/xml_generator.hpp"

namespace standardese::model {

model::entity document::block() const {
  if (children.begin() == children.end())
    return model::markup::paragraph{};

  if (++children.begin() != children.end())
    logger::warn(fmt::format("Requested unique block of a document but it contains more than one block. Ignoring trailing blocks in document {}", output_generator::xml::xml_generator::render(*this)));

  return *children.begin();
}

model::markup::paragraph document::paragraph() const {
  if (children.begin() == children.end())
    return {};

  if (++children.begin() != children.end())
    logger::warn(fmt::format("Requested unique paragraph of a document but it contains more than one block. Ignoring trailing blocks in document {}", output_generator::xml::xml_generator::render(*this)));

  if (!children.begin()->is<model::markup::paragraph>()) {
    logger::error(fmt::format("Requested unique paragraph of a document but {} does not start with a paragraph.", output_generator::xml::xml_generator::render(*this)));
    return {};
  }

  return children.begin()->as<model::markup::paragraph>();
}

}
