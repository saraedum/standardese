// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include "../../../include/standardese/output/markup/subheading.hpp"

namespace standardese::output::markup
{

entity_kind subheading::do_get_kind() const noexcept
{
    return entity_kind::subheading;
}

void subheading::do_visit(visitor_callback_t cb, void* mem) const
{
    for (auto& child : *this)
        cb(mem, child);
}

std::unique_ptr<entity> subheading::do_clone() const
{
    builder b(id());
    for (auto& child : *this)
        b.add_child(detail::unchecked_downcast<phrasing_entity>(child.clone()));
    return b.finish();
}

}
