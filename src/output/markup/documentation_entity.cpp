// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include "../include/standardese/output/markup/documentation_entity.hpp"

namespace standardese::output::markup
{

type_safe::optional_ref<const standardese::output::markup::brief_section> documentation_entity::
    brief_section() const noexcept
{
    for (auto& section : sections_)
        if (section->kind() == entity_kind::brief_section)
            return type_safe::ref(static_cast<const markup::brief_section&>(*section));
    return nullptr;
}

type_safe::optional_ref<const standardese::output::markup::details_section> documentation_entity::
    details_section() const noexcept
{
    for (auto& section : sections_)
        if (section->kind() == entity_kind::details_section)
            return type_safe::ref(static_cast<const markup::details_section&>(*section));
    return nullptr;
}

}
