// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//               2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_MODEL_SECTION_SECTION_HPP_INCLUDED
#define STANDARDESE_MODEL_SECTION_SECTION_HPP_INCLUDED

#include "mixin/visitable.hpp"
#include "mixin/container.hpp"

namespace standardese::model
{
    /// A named section like `\effects` or `\returns`.
    /// This cannot be used for brief or details.
    /// TODO: There is no good reason for this legacy anymore. Drop this limitation.
    class section final : public mixin::visitable<section>, public mixin::container<>
    {
    public:
        template <typename ...Args>
        section(parser::commands::section_command type, Args&&... args)
        : type(type), mixin::container<>(std::forward<Args>(args)...)
        {}

        parser::commands::section_command type;
    };
}

#endif

