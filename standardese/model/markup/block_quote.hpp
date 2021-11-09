// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_MODEL_MARKUP_BLOCK_QUOTE_HPP_INCLUDED
#define STANDARDESE_MODEL_MARKUP_BLOCK_QUOTE_HPP_INCLUDED

#include "../mixin/container.hpp"
#include "../mixin/visitable.hpp"

namespace standardese::model::markup
{
    /// A block quote.
    class block_quote final : public mixin::container<entity>, public mixin::visitable<block_quote>
    {
    public:
        using mixin::container<entity>::container;
    };
}

#endif

