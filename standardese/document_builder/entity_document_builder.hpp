// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_OUTPUT_DOCUMENT_ENTITY_DOCUMENT_BUILDER_HPP_INCLUDED
#define STANDARDESE_OUTPUT_DOCUMENT_ENTITY_DOCUMENT_BUILDER_HPP_INCLUDED

#include <string>

#include "../forward.hpp"

namespace standardese::document_builder
{

/// Creates a [model::document]() describing a C++ entity such as a header file.
class entity_document_builder {
  public:
    /// Create a document describing `entity`.
    /// \param name The name of the generated document, e.g., `vector`.
    /// \param path The eventual path of the final output generated from the
    /// document for linking, typically the same as [name]().
    model::document build(const std::string& name, const std::string& path, const model::entity& entity, const model::unordered_entities&) const;
};

}

#endif
