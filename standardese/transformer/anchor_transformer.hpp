// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_TRANSFORMER_ANCHOR_TRANSFORMER_HPP_INCLUDED
#define STANDARDESE_TRANSFORMER_ANCHOR_TRANSFORMER_HPP_INCLUDED

#include "transformer.hpp"

namespace standardese::transformer {

// TODO(0.6.0-alpha): This should be called identifier transformer. Clean up the mess we made with the "anchor" term. We should not use it. Use link instead.
class anchor_transformer : public transformer {
  public:
    using transformer::transformer;

  protected:
    void do_transform(model::entity& root) override;
};

}

#endif
