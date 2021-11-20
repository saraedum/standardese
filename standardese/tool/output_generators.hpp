// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//               2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_TOOL_OUTPUT_GENERATORS_HPP_INCLUDED
#define STANDARDESE_TOOL_OUTPUT_GENERATORS_HPP_INCLUDED

#include <string>
#include <boost/filesystem/path.hpp>

#include "../output_generator/markdown/markdown_generator.hpp"

namespace standardese::tool {

/// Writes documents as output files to disk.
/// This wraps all the output generators that the standardese executable uses.
class output_generators {
 public:
  struct output_generators_options {
    enum class output_format {
      markdown,
      html,
      text,
      xml,
    };

    /// The format we emit documentation in.
    /// Supplementary files, such as inventories, might be generated in
    /// additional formats.
    output_format primary_format = output_format::markdown;

    /// The name of the intersphinx inventory to create.
    /// No inventory is created if this is the empty string.
    std::string intersphinx_inventory = "objects.inv";

    /// The name of the doxygen tagfile to create.
    /// No tagfile is created if this is the empty string.
    std::string doxygen_tagfile = "tagfile.xml";

    /// Options affecting MarkDown output if [primary_format]() is set to
    /// `markdown`.
    output_generator::markdown::markdown_generator::markdown_generator_options markdown_options;

    /// The directory to build the output files in.
    /// The directory is created (but not cleared) if it does not already
    /// exist.
    boost::filesystem::path output_directory = "standardese";
  };

  output_generators(output_generators_options);

  /// Write the output files.
  void emit(model::unordered_entities& documents);

 private:
  struct output_generators_options options;
};

}

#endif
