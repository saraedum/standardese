// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_MODEL_MIXIN_ANCHORED_HPP_INCLUDED
#define STANDARDESE_MODEL_MIXIN_ANCHORED_HPP_INCLUDED

#include <string>

namespace standardese::model::mixin
{
    /// Implements the `anchored_entity` concept, i.e., this entity can be linked to.
    class anchored
    {
    public:
        std::string id;
    };
}

#endif

