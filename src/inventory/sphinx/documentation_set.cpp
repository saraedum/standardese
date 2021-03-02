// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>

#include <fmt/format.h>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include <boost/iostreams/copy.hpp>
#include <stdexcept>

#include "../../../standardese/inventory/sphinx/documentation_set.hpp"
#include "../../../standardese/logger.hpp"

namespace standardese::inventory::sphinx {

namespace {

void load_v1(std::istream& in, documentation_set& inventory);
void load_v2(std::istream& in, documentation_set& inventory);

}

documentation_set documentation_set::parse(const std::string& fname) {
  // TODO: Implement better error detection/handling.
  documentation_set set;
  std::ifstream(fname) >> set;
  return set;
}

std::ostream& operator<<(std::ostream& out, const documentation_set& inventory) {
  out << "# Sphinx inventory version 2" << std::endl;
  out << fmt::format("# Project: {}", inventory.project) << std::endl;
  out << fmt::format("# Version: {}", inventory.version) << std::endl;
  out << "# The remainder of this file is compressed using zlib." << std::endl;

  boost::iostreams::filtering_streambuf<boost::iostreams::output> compressed;
  compressed.push(boost::iostreams::zlib_compressor());
  compressed.push(out);

  std::stringstream uncompressed;

  for (const auto& entry : inventory.entries)
    uncompressed << fmt::format("{} {}:{} {} {} {}", entry.name, entry.domain, entry.type, entry.priority, entry.uri, entry.display_name) << std::endl;

  uncompressed << std::flush;

  boost::iostreams::copy(uncompressed, compressed);

  return out;
}

std::istream& operator>>(std::istream& in, documentation_set& inventory) {
  std::string header;

  if (in.bad())
    throw std::invalid_argument("Cannot read inventory from bad stream.");

  if (in.eof())
    throw std::invalid_argument("Cannot read inventory from closed stream.");

  std::getline(in, header);

  // See https://github.com/sphinx-doc/sphinx/blob/4.x/sphinx/util/inventory.py#L87 for the supported headers.
  if (header == "# Sphinx inventory version 1") {
    load_v1(in, inventory);
  } else if (header == "# Sphinx inventory version 2") {
    load_v2(in, inventory);
  } else {
    throw std::invalid_argument("Sphinx inventory not in a supported format: `" + header + "`");
  }

  return in;
}

namespace {

// See https://github.com/sphinx-doc/sphinx/blob/4.x/sphinx/util/inventory.py#L95
void load_v1(std::istream& in, documentation_set& inventory) {
  throw std::logic_error("not implemented: loading of Sphinx version 1 inventories"); 
}

// See https://github.com/sphinx-doc/sphinx/blob/4.x/sphinx/util/inventory.py#L113
void load_v2(std::istream& in, documentation_set& inventory) {
  std::getline(in, inventory.project);
  inventory.project = inventory.project.substr(11);

  std::getline(in, inventory.version);
  inventory.version = inventory.version.substr(11);

  // Following the implementation at https://github.com/sphinx-doc/sphinx/blob/4.x/sphinx/util/inventory.py#L118
  std::string header;
  std::getline(in, header);

  static std::regex zlib{"zlib"};
  if (!std::regex_search(header, zlib))
    throw std::invalid_argument("invalid sphinx inventory header (not compressed)");

  boost::iostreams::filtering_streambuf<boost::iostreams::input> compressed;
  compressed.push(boost::iostreams::zlib_decompressor());
  compressed.push(in);

  std::stringstream uncompressed;

  boost::iostreams::copy(compressed, uncompressed);

  while(!uncompressed.eof()) {
    std::string line;
    std::getline(uncompressed, line);

    // Essentially the same as https://github.com/sphinx-doc/sphinx/blob/4.x/sphinx/util/inventory.py#L123
    static std::regex syntax{R"((.+?)\s+(\S+)\s+(-?\d+)\s+?(\S*)\s+(.*)\s*)"};
    std::smatch match;

    if (!std::regex_match(line, match, syntax)) {
      // Apparently, non-matching lines are allowed: https://github.com/sphinx-doc/sphinx/blob/4.x/sphinx/util/inventory.py#L126
      logger::info(fmt::format("Ignoring malformed line in Sphinx inventory: `{}`", line));
      continue;
    }

    assert(match.size() == 6);

    std::string name = match[1].str();
    std::string type = match[2].str();
    std::string prio = match[3].str();
    std::string location = match[4].str();
    std::string dispname = match[5].str();

    if (type.find(':') == std::string::npos) {
      // Incorrectly formatted types are ignored: https://github.com/sphinx-doc/sphinx/blob/4.x/sphinx/util/inventory.py#L128
      logger::info(fmt::format("Ignoring malformed domain:type in Sphinx inventory: `{}`", type));
      continue;
    }

    std::string domain = type.substr(0, type.find(':'));

    if (type == "py:module") {
      // Work around a bug in Sphinx 1.1, see https://github.com/sphinx-doc/sphinx/blob/4.x/sphinx/util/inventory.py#L133
      if (std::find_if(begin(inventory.entries), end(inventory.entries), [&](const auto& entry) { return entry.name == name; }) != end(inventory.entries))
        continue;
    }

    type = type.substr(type.find(':') + 1);

    if (location.size() && location[location.size() - 1] == '$')
      location = location.substr(0, location.size() - 1) + name;

    inventory.entries.emplace_back(name, domain, type, atol(prio.c_str()), location, dispname);
  }
}

}

}
