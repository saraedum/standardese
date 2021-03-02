// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>

#include <cppast/cpp_file.hpp>
#include <type_safe/optional.hpp>

#include "../../standardese/inventory/files.hpp"

namespace standardese::inventory {

files::files(const std::vector<const cppast::cpp_file*>& headers): headers(begin(headers), end(headers)) {}

type_safe::optional_ref<const cppast::cpp_file> files::find_header(const std::string& name) const {
  return find_header(name, boost::filesystem::current_path().native());
}

type_safe::optional_ref<const cppast::cpp_file> files::find_header(const std::string& name, const std::string& path) const {
  // TODO: Make this lookup more flexible.
  for (const auto* header : headers) {
    if (boost::filesystem::path(header->name()).filename() == boost::filesystem::path(name).filename())
      return type_safe::ref(*header);
  }

  return type_safe::nullopt;
}

}
