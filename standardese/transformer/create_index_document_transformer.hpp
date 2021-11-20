// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_TRANSFORMER_CREATE_INDEX_DOCUMENT_TRANSFORMER_HPP_INCLUDED
#define STANDARDESE_TRANSFORMER_CREATE_INDEX_DOCUMENT_TRANSFORMER_HPP_INCLUDED

#include <functional>
#include <string>

#include "../formatter/inja_formatter.hpp"
#include "outer_transformer.hpp"

namespace standardese::transformer {

/// Generates indexes of entities, i.e., documents that essentially contain a
/// list of all entities of a certain kind such as a list of all headers.
class create_index_document_transformer {
  public:
    struct create_index_document_transformer_options {
      create_index_document_transformer_options();

      /// The name of the generated document, e.g., `headers`.
      std::string name = "standardese_entities";

      /// The eventual path of the final output generated from the document for
      /// linking, typically the same as [name]().
      std::string path = "standardese_entities";

      /// The index will be generated for all entities that satisfy this
      /// predicate. The default is to create an index of all entities.
      std::function<bool(const model::entity&)> predicate;

      struct formatter::inja_formatter::inja_formatter_options target_text_options;
    };

    /// Create a transformer that creates a single index document listing all
    /// the [entities]() that match
    /// [create_index_document_transformer_options::predicate]().
    create_index_document_transformer(const model::entity_set* entities, const parser::cpp_context&, create_index_document_transformer_options);

    /// Create an index of all entities satisfying `predicate`.
    model::document transform(threading::pool::factory workers=threading::unthreaded_pool::factory) const;

  protected:
    /// Return true if the argument is a module.
    /// This predicate can be used in [build]() to generate an index of all modules.
    static bool is_module(const model::entity&);

    /// Return true if the argument is a header file.
    /// This predicate can be used in [build]() to generate an index of all header files.
    static bool is_header_file(const model::entity&);

  private:
    formatter::inja_formatter target_text_formatter;

    create_index_document_transformer_options options;

    const model::entity_set* entities;
};

}

#endif
