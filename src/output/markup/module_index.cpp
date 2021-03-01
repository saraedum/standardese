// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include "../../../include/standardese/output/markup/module_index.hpp"

namespace standardese::output::markup
{

entity_kind module_index::do_get_kind() const noexcept
{
    return entity_kind::module_index;
}

void module_index::do_visit(visitor_callback_t cb, void* mem) const
{
    cb(mem, heading());
    for (auto& child : *this)
        cb(mem, child);
}

std::unique_ptr<entity> module_index::do_clone() const
{
    builder b(detail::unchecked_downcast<markup::heading>(heading().clone()));
    for (auto& child : *this)
        b.add_child(detail::unchecked_downcast<module_documentation>(child.clone()));
    return b.finish();
}

}


