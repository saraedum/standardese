// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_OUTPUT_DOCUMENT_INDEX_DOCUMENT_BUILDER_HPP_INCLUDED
#define STANDARDESE_OUTPUT_DOCUMENT_INDEX_DOCUMENT_BUILDER_HPP_INCLUDED

#include <functional>

#include "../forward.hpp"

namespace standardese::document_builder
{

class index_document_builder {
  public:
    /// Create an index of all entities satisfying `predicate`.
    model::document build(const std::function<bool(const model::entity&)> predicate, const model::unordered_entities&) const;

    static bool is_module(const model::entity&);
    static bool is_header_file(const model::entity&);
};

}

#endif
