// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//               2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_MODEL_MARKUP_ORDERED_LIST_HPP_INCLUDED
#define STANDARDESE_MODEL_MARKUP_ORDERED_LIST_HPP_INCLUDED

#include "../mixin/container.hpp"
#include "../mixin/visitable.hpp"
#include "list_item.hpp"

namespace standardese::model::markup
{
    /// An ordered list of items.
    class list final : public mixin::container<list_item>, public mixin::visitable<list>
    {
    public:
        explicit list(bool ordered=false, std::initializer_list<list_item> children={}) : ordered(ordered), mixin::container<list_item>(std::move(children)) {}

        bool ordered;
    };
}

#endif
