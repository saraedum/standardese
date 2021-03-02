// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_MODEL_MARKUP_PARAGRAPH_HPP_INCLUDED
#define STANDARDESE_MODEL_MARKUP_PARAGRAPH_HPP_INCLUDED

#include "../mixin/visitable.hpp"
#include "../mixin/container.hpp"

namespace standardese::model::markup
{
    /// A plain paragraph.
    class paragraph final : public mixin::visitable<paragraph>, public mixin::container<>
    {
    public:
        using mixin::container<>::container;
    };
}

#endif
