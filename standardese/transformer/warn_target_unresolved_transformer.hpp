// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_TRANSFORMER_WARN_TARGET_UNRESOLVED_TRANSFORMER_HPP_INCLUDED
#define STANDARDESE_TRANSFORMER_WARN_TARGET_UNRESOLVED_TRANSFORMER_HPP_INCLUDED

#include "inner_transformer.hpp"
#include "../forward.hpp"

namespace standardese::transformer
{

/// Handle links in MarkDown that use standardese syntax but could not be
/// resolved by emitting warnings for them.
class warn_target_unresolved_transformer : public inner_transformer {
  public:
    using inner_transformer::inner_transformer;

  protected:
    void do_transform(model::entity&) override;
};

}

#endif
