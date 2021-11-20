// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <iostream>
#include <cassert>
#include <fmt/format.h>

#include <pugixml.hpp>

#include "../../../standardese/inventory/doxygen/tagfile.hpp"
#include "../../../standardese/logger.hpp"
#include "../../../standardese/output_generator/xml/xml_generator.hpp"

namespace standardese::inventory::doxygen {

namespace {

void write(pugi::xml_node&, const tagfile&);

void write(pugi::xml_node& document, const tagfile& tagfile) {
  auto parent = output_generator::xml::xml_generator::append_child(document, "tagfile");

  // TODO(0.6.0-beta): implement me.
}

}

std::istream& operator>>(std::istream&, tagfile&) {
  // TODO(0.6.0-beta): implement me
  throw std::logic_error("not implemented: istream>>tagfile");
}

std::ostream& operator<<(std::ostream& out, const tagfile& tagfile) {
  // The root of the output XML document.
  pugi::xml_document xml_document;

  write(xml_document, tagfile);

  xml_document.save(out, "  ");

  return out;
}

}
