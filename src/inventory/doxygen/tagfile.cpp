// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <iostream>
#include <cassert>
#include <fmt/format.h>

#include <pugixml.hpp>

#include "../../../standardese/inventory/doxygen/tagfile.hpp"
#include "../../../standardese/logger.hpp"

namespace standardese::inventory::doxygen {

namespace {

void write(pugi::xml_node&, const tagfile&);

pugi::xml_node append_child(pugi::xml_node& parent, const std::string& name) {
  auto node = parent.append_child(name.c_str());

  if (node.empty())
    logger::error(fmt::format("Could not create {} node in doxygen tagfile output.", name));

  return node;
}

void set_attribute(pugi::xml_node& node, const std::string& name, const std::string& value) {
  auto attribute = node.append_attribute(name.c_str());

  if (attribute.empty())
    logger::error(fmt::format("Could not create attribute {} with value {} in doxygen tagfile output.", name, value));

  if (value.size())
    if (!attribute.set_value(value.c_str()))
      logger::error(fmt::format("Could not set attribute {} to value {} in doxygen tagfile output.", name, value));
}

void write(pugi::xml_node& document, const tagfile& tagfile) {
  auto parent = append_child(document, "tagfile");

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
