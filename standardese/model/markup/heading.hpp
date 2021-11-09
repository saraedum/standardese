// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_MODEL_MARKUP_HEADING_HPP_INCLUDED
#define STANDARDESE_MODEL_MARKUP_HEADING_HPP_INCLUDED

#include "text.hpp"
#include "../mixin/visitable.hpp"
#include "../mixin/anchored_container.hpp"

namespace standardese::model::markup
{
    // TODO: It's probably good if virtually all entities had something like a
    // "source". Explaining what made them so we can properly render things
    // out, e.g., when rendering for YAML.
    class heading final : public mixin::anchored_container<>, public mixin::visitable<heading>
    {
    public:
        template <typename ...Args>
        explicit heading(int level, Args&&... children) : level(level), mixin::anchored_container<>(std::forward<Args>(children)...) {}

        int level;
    };

}

#endif
