// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_MODEL_MARKUP_CODE_BLOCK_HPP_INCLUDED
#define STANDARDESE_MODEL_MARKUP_CODE_BLOCK_HPP_INCLUDED

#include "../mixin/container.hpp"
#include "../mixin/visitable.hpp"

namespace standardese::model::markup
{
    class code_block final : public mixin::container<>, public mixin::visitable<code_block>
    {

    public:
        template <typename ...Args>
        explicit code_block(std::string language, Args&&... children)
        : language(std::move(language)), mixin::container<>{std::forward<Args>(children)...}
        {}

        std::string language;
    };
}

#endif
