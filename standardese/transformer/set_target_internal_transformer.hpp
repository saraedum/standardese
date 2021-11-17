// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_TRANSFORMER_SET_TARGET_INTERNAL_TRANSFORMER_HPP_INCLUDED
#define STANDARDESE_TRANSFORMER_SET_TARGET_INTERNAL_TRANSFORMER_HPP_INCLUDED

#include "inner_transformer.hpp"
#include "../inventory/cppast_inventory.hpp"
#include "../inventory/files.hpp"

namespace standardese::transformer
{

/// Resolves links in MarkDown that use standardese syntax to link to targets within the same project.
class set_target_internal_transformer : public inner_transformer {
  public:
    /// Create a transformer that will rewrite each
    /// [model::markup::link::target]() attribute in all the [documents]() for
    /// links that refer to targets within the same project.
    set_target_internal_transformer(model::unordered_entities* documents, const parser::cpp_context& context);

  protected:
    void do_transform(model::entity&) override;

  private:
    inventory::cppast_inventory inventory;
    inventory::files files;
};

}

#endif

