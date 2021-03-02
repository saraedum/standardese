// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_INVENTORY_SPHINX_ENTRY_HPP_INCLUDED
#define STANDARDESE_INVENTORY_SPHINX_ENTRY_HPP_INCLUDED

#include <iosfwd>
#include <string>
#include <vector>
#include <type_safe/optional.hpp>

#include "../../forward.hpp"

namespace standardese::inventory::sphinx {

/// A line in an intersphinx documentation set inventory.
/// See
/// [inventory.py](https://github.com/sphinx-doc/sphinx/blob/4.x/sphinx/util/inventory.py#L147)
/// for the place in sphinx where these lines are rendered.
class entry {
 public:
  entry(std::string name, std::string domain, std::string type, long priority, std::string uri, std::string display_name);

  // TODO: We could be more restrictive with the typing here?
  std::string name;
  std::string domain;
  std::string type;
  long priority;
  std::string uri;
  std::string display_name;
};

}

#endif

