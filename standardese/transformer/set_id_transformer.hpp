// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_TRANSFORMER_SET_ID_TRANSFORMER_HPP_INCLUDED
#define STANDARDESE_TRANSFORMER_SET_ID_TRANSFORMER_HPP_INCLUDED

#include "inner_transformer.hpp"

namespace standardese::transformer {

// TODO(0.6.0-alpha): Clean up the mess we made with the "anchor" term. We should not use it. Use link instead.

/// Establishes the [model::mixin::anchored::id]() for each entity that can be
/// linked to in the output documents.
class set_id_transformer : public inner_transformer {
  public:
    using inner_transformer::inner_transformer;

  protected:
    void do_transform(model::entity& root) override;
};

}

#endif
