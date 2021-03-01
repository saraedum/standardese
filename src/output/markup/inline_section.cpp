// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include "../../../include/standardese/output/markup/inline_section.hpp"

namespace standardese::output::markup
{

entity_kind inline_section::do_get_kind() const noexcept
{
    return entity_kind::inline_section;
}

void inline_section::do_visit(visitor_callback_t cb, void* mem) const
{
    for (auto& child : *paragraph_)
        cb(mem, child);
}

std::unique_ptr<entity> inline_section::do_clone() const
{
    builder b(type_, name());
    for (auto& child : *paragraph_)
        b.add_child(detail::unchecked_downcast<phrasing_entity>(child.clone()));
    return b.finish();
}

}
