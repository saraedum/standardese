// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//               2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_MODEL_MARKUP_LIST_ITEM_HPP_INCLUDED
#define STANDARDESE_MODEL_MARKUP_LIST_ITEM_HPP_INCLUDED

#include "../mixin/container.hpp"
#include "../mixin/visitable.hpp"

namespace standardese::model::markup
{
    /// An item in a list.
    ///
    /// This is the HTML `<li>` container.
    class list_item final : public mixin::container<>, public mixin::visitable<list_item>
    {
    public:
        using mixin::container<entity>::container;
    };
}

#endif
