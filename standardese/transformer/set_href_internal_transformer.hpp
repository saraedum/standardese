// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_TRANSFORMER_SET_HREF_INTERNAL_TRANSFORMER_HPP_INCLUDED
#define STANDARDESE_TRANSFORMER_SET_HREF_INTERNAL_TRANSFORMER_HPP_INCLUDED

#include <unordered_map>

#include "inner_transformer.hpp"
#include "../parser/cpp_context.hpp"

namespace standardese::transformer
{

/// Rewrites MarkDown links that target a C++ entity as a link to an actual URL.
class set_href_internal_transformer : public inner_transformer {
  public:
    /// Create a transformer that sets all the [model::markup::link::target]()
    /// in each of the [documents]() if they refer to targets in the same
    /// project.
    set_href_internal_transformer(model::entity_set* documents, const parser::cpp_context&);

  protected:
    void do_transform(model::entity&) override;

  private:
    std::unordered_map<const cppast::cpp_entity*, std::string> targets;
    parser::cpp_context context;
};

}

#endif
