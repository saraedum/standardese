// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_TRANSFORMER_CREATE_ENTITY_DOCUMENT_TRANSFORMER_HPP_INCLUDED
#define STANDARDESE_TRANSFORMER_CREATE_ENTITY_DOCUMENT_TRANSFORMER_HPP_INCLUDED

#include <string>
#include <cppast/cppast_fwd.hpp>

#include "outer_transformer.hpp"
#include "../parser/cpp_context.hpp"

namespace standardese::transformer {

/// Creates a [model::document]() describing a C++ entity such as a header file.
class create_entity_document_transformer : public outer_transformer {
  public:
    struct create_entity_document_transformer_options {
      create_entity_document_transformer_options();

      /// Create entity documents for the entities that pass this filter.
      /// The default is to create entity documents for all header files.
      std::function<bool(const cppast::cpp_entity&)> filter;

      /// The name of the output file (without the suffix).
      /// The default is to take a cleaned-up name of what we are documenting.
      std::string document_name = "doc_{{ sanitize_basename(relative(paths)) }}";
  
      /// The name of the final rendered (HTML) document.
      /// Links to this document will assume that this is the eventual (absolute)
      /// path of this document.
      /// The default is configured just like [name]() from above and assumes
      /// that everything lives under the same document root without extensions
      /// such as `.html`.
      std::string document_path = "doc_{{ sanitize_basename(relative(paths)) }}";
    };

    /// Create a transformer that will create a document for each entity in
    /// [entities]() that passes the predicate
    /// [create_entity_document_transformer_options::filter]().
    create_entity_document_transformer(const model::unordered_entities* entities, const parser::cpp_context&, create_entity_document_transformer_options={});

  protected:
    virtual std::vector<model::entity> do_transform(const model::entity& entity) override;

    /// Create a document describing `entity`.
    /// \param name The name of the generated document, e.g., `vector`.
    /// \param path The eventual path of the final output generated from the
    /// document for linking, typically the same as [name]().
    model::document build(const std::string& name, const std::string& path, const model::entity& entity) const;

  private:
    create_entity_document_transformer_options options;

    std::vector<std::string> headers;

    parser::cpp_context context;
};

}

#endif
