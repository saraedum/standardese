// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_INVENTORY_FILES_HPP_INCLUDED
#define STANDARDESE_INVENTORY_FILES_HPP_INCLUDED

#include <unordered_set>

#include <cppast/forward.hpp>
#include <type_safe/optional_ref.hpp>

namespace standardese::inventory {

/// An inventory of input files that standardese parsed and output files that standardese generates.
/// This allows fuzzy lookups of files by name similar to [symbols]() which
/// lets us search for C++ entities by name.
class files {
 public:
  explicit files(const std::vector<const cppast::cpp_file*>&);

  /// Return the header file `name`.
  /// Note that this search is a bit fuzzy. We first try to make sense of
  /// `name` relative to the current working directory. If this does not yield
  /// anything, we return any header ending in `name`.
  type_safe::optional_ref<const cppast::cpp_file> find_header(const std::string& name) const;

  /// Return the header file `name` taking into account path names relative
  /// to the definition of the `entity`.
  /// Note that this search is a bit fuzzy. We first try to make sense of
  /// `name` relative to the directory containing `entity`. If this does not
  /// yield anything, we return any header ending in `name`.
  type_safe::optional_ref<const cppast::cpp_file> find_header(const std::string& name, const cppast::cpp_entity& entity) const;

  /// Return the header `name` searching from the location `path`.
  type_safe::optional_ref<const cppast::cpp_file> find_header(const std::string& name, const std::string& path) const;

 private:
  std::unordered_set<const cppast::cpp_file*> headers;
};

}

#endif
