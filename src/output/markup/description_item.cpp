// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include "../../../include/standardese/output/markup/term_description_item.hpp"

namespace standardese::output::markup
{

entity_kind term_description_item::do_get_kind() const noexcept
{
    return entity_kind::term_description_item;
}

void term_description_item::do_visit(visitor_callback_t cb, void* mem) const
{
    cb(mem, term());
    cb(mem, description());
}

std::unique_ptr<entity> term_description_item::do_clone() const
{
    return build(id(), detail::unchecked_downcast<markup::term>(term().clone()),
                 detail::unchecked_downcast<markup::description>(description().clone()));
}

}

