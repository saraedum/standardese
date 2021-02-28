// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//               2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_OUTPUT_MARKUP_LIST_ITEM_BASE_HPP_INCLUDED
#define STANDARDESE_OUTPUT_MARKUP_LIST_ITEM_BASE_HPP_INCLUDED

#include "block_entity.hpp"

namespace standardese::output::markup
{
    /// The base class for all items in a list.
    class list_item_base : public block_entity
    {
    protected:
        using block_entity::block_entity;
    };
}

#endif


