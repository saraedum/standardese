// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_MODEL_MARKUP_EMPHASIS_HPP_INCLUDED
#define STANDARDESE_MODEL_MARKUP_EMPHASIS_HPP_INCLUDED

#include "../mixin/container.hpp"
#include "text.hpp"
#include "../mixin/visitable.hpp"

namespace standardese::model::markup
{
    /// A fragment that is emphasized.
    ///
    /// This corresponds to the `<em>` HTML tag.
    class emphasis final : public mixin::container<>, public mixin::visitable<emphasis>
    {
    public:
        using mixin::container<>::container;
    };
}

#endif

