// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_TRANSFORMATION_LINK_TARGET_UNRESOLVED_TRANSFORMATION_HPP_INCLUDED
#define STANDARDESE_TRANSFORMATION_LINK_TARGET_UNRESOLVED_TRANSFORMATION_HPP_INCLUDED

#include "transformation.hpp"
#include "../forward.hpp"

namespace standardese::transformation
{

/// Handle links in MarkDown that use standardese syntax but could not be
/// resolved by other means, e.g., by emitting warnings for them.
class link_target_unresolved_transformation : public transformation {
  public:
    link_target_unresolved_transformation(model::unordered_entities& documents);

  protected:
    void do_transform(model::entity&) override;
};

}

#endif
