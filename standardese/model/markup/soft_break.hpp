// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_MODEL_MARKUP_SOFT_BREAK_HPP_INCLUDED
#define STANDARDESE_MODEL_MARKUP_SOFT_BREAK_HPP_INCLUDED

#include "../mixin/visitable.hpp"

namespace standardese::model::markup
{
    /// A soft line break.
    class soft_break final : public mixin::visitable<soft_break>
    {};
}

#endif

