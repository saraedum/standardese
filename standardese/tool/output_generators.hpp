// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//               2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_TOOL_OUTPUT_GENERATORS_HPP_INCLUDED
#define STANDARDESE_TOOL_OUTPUT_GENERATORS_HPP_INCLUDED

#include <string>
#include <boost/filesystem/path.hpp>

#include "../model/unordered_entities.hpp"
#include "../output_generator/markdown/markdown_generator.hpp"

namespace standardese::tool {

/// Writes documents as output files to disk.
/// This wraps all the output generators that the standardese executable uses.
class output_generators {
 public:
  struct options {
    enum class output_format {
      markdown,
      html,
      text,
      xml,
    };

    // TODO: Implement me
    /// The format actual documentation is emitted in.
    /// Supplementary files, such as inventories, might be generated in
    /// additional formats.
    output_format primary_format = output_format::markdown;

    // TODO: Implement me
    /// The name of the intersphinx inventory to create.
    /// No inventory is created if empty.
    std::string intersphinx_inventory = "objects.inv";

    // TODO: Implement me
    /// The name of the doxygen tagfile to create.
    /// No tagfile is created if empty.
    std::string doxygen_tagfile = "tagfile.xml";

    /// Options affecting MarkDown output if [primary_format]() is set to `markdown`.
    struct output_generator::markdown::markdown_generator::options markdown_options; 

    // TODO: Implement me
    /// Directory the hierarchy of output files is built in.
    /// The directory is created (but not cleared) if it does not already
    /// exist.
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
