// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_OUTPUT_DOCUMENT_INDEX_DOCUMENT_BUILDER_HPP_INCLUDED
#define STANDARDESE_OUTPUT_DOCUMENT_INDEX_DOCUMENT_BUILDER_HPP_INCLUDED

#include <functional>
#include <string>

#include "../formatter/inja_formatter.hpp"

namespace standardese::document_builder
{

/// Generates indexes of entities, i.e., documents that essentially contain a
/// list of all entities of a certain kind such as a list of all headers.
class index_document_builder {
  public:
    struct options {
      options();

      struct formatter::inja_formatter::inja_formatter_options anchor_text_options;
    };

    index_document_builder(options, parser::cpp_context);

    /// Create an index of all entities satisfying `predicate`.
    /// \param name The name of the generated document, e.g., `headers`.
    /// \param path The eventual path of the final output generated from the
    /// document for linking, typically the same as [name]().
    model::document build(const std::string& name, const std::string& path, const std::function<bool(const model::entity&)> predicate, const model::unordered_entities&) const;

    /// Return true if the argument is a module.
    /// This predicate can be used in [build]() to generate an index of all modules.
    static bool is_module(const model::entity&);

    /// Return true if the argument is a header file.
    /// This predicate can be used in [build]() to generate an index of all header files.
    static bool is_header_file(const model::entity&);

  private:
    formatter::inja_formatter anchor_text_formatter;
};

}

#endif
