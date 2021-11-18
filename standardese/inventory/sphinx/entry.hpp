// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_INVENTORY_SPHINX_ENTRY_HPP_INCLUDED
#define STANDARDESE_INVENTORY_SPHINX_ENTRY_HPP_INCLUDED

#include <iosfwd>
#include <string>
#include <vector>

#include "../../forward.hpp"

namespace standardese::inventory::sphinx {

/// A line in an intersphinx documentation set inventory.
/// See
/// [inventory.py](https://github.com/sphinx-doc/sphinx/blob/4.x/sphinx/util/inventory.py#L147)
/// for the place in sphinx where these lines are rendered. See also the
/// [domains
/// module](https://github.com/sphinx-doc/sphinx/blob/4.x/sphinx/domains/__init__.py#L355).
class entry {
 public:
  /// Create a doxygen entry.
  entry(std::string name, std::string domain, std::string type, long priority, std::string uri, std::string display_name);

  /// The fully-qualified
  /// [name](https://github.com/sphinx-doc/sphinx/blob/4.x/sphinx/domains/__init__.py#L360)
  /// of this entity.
  std::string name;

  /// The document where this entity ["is to be
  /// found"](https://github.com/sphinx-doc/sphinx/blob/4.x/sphinx/domains/__init__.py#L369).
  std::string domain;

  /// The
  /// [type](https://github.com/sphinx-doc/sphinx/blob/4.x/sphinx/domains/__init__.py#L366)
  /// of this entity.
  std::string type;

  /// The
  /// [priority](https://github.com/sphinx-doc/sphinx/blob/4.x/sphinx/domains/__init__.py#L375)
  /// of this entity.
  long priority;

  /// The [anchor
  /// name](https://github.com/sphinx-doc/sphinx/blob/4.x/sphinx/domains/__init__.py#L372)
  /// of this entity.
  std::string uri;

  /// The
  /// [name](https://github.com/sphinx-doc/sphinx/blob/4.x/sphinx/domains/__init__.py#L363)
  /// to display when linking to this entity.
  std::string display_name;
};

}

#endif

