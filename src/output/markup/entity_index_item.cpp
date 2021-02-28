// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include "../../../include/standardese/output/markup/entity_index_item.hpp"

namespace standardese::output::markup
{

entity_kind entity_index_item::do_get_kind() const noexcept
{
    return entity_kind::entity_index_item;
}

void entity_index_item::do_visit(visitor_callback_t cb, void* mem) const
{
    cb(mem, entity());
    if (brief())
        cb(mem, brief().value());
}

std::unique_ptr<entity> entity_index_item::do_clone() const
{
    return build(id(), detail::unchecked_downcast<term>(entity().clone()),
                 brief() ? detail::unchecked_downcast<description>(brief().value().clone())
                         : nullptr);
}

}
