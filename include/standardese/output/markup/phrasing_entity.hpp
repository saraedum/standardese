// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_OUTPUT_MARKUP_PHRASING_ENTITY_HPP_INCLUDED
#define STANDARDESE_OUTPUT_MARKUP_PHRASING_ENTITY_HPP_INCLUDED

#include "entity.hpp"

namespace standardese::output::markup
{
    /// Base class for all phrasing entities.
    ///
    /// A pharsing entity adds structural information like emphasis to text fragments.
    class phrasing_entity : public entity
    {
    protected:
        phrasing_entity() noexcept = default;
    };
}

#endif
