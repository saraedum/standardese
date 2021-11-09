// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//               2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_TOOL_OUTPUT_GENERATORS_HPP_INCLUDED
#define STANDARDESE_TOOL_OUTPUT_GENERATORS_HPP_INCLUDED

#include <string>
#include <boost/filesystem/path.hpp>

#include "../model/unordered_entities.hpp"

namespace standardese::tool {

/// Writes documents as output files to disk.
/// This wraps all the output generators that the standardese executable uses.
class output_generators {
 public:
  struct options {
    boost::filesystem::path output_directory = "standardese";
  };

  output_generators(struct options);

  /// Write the output files.
  void emit(model::unordered_entities& documents);

 private:
  struct options options;
};

}

#endif
