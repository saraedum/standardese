// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include "../../../include/standardese/output/markup/soft_break.hpp"

namespace standardese::output::markup
{

entity_kind soft_break::do_get_kind() const noexcept
{
    return entity_kind::soft_break;
}

void soft_break::do_visit(visitor_callback_t, void*) const {}

std::unique_ptr<entity> soft_break::do_clone() const
{
    return build();
}

}
