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
    /// A named section like `\effects` or `\returns` or an implicit section
    /// such as the brief or the details.
    class section final : public mixin::visitable<section>, public mixin::container<>
    {
    public:
        explicit section(parser::commands::section_command type, std::initializer_list<model::entity> children={})
        : type(type), mixin::container<>(std::move(children))
        {}

        parser::commands::section_command type;
    };
}

#endif

