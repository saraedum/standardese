// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include "../../../include/standardese/output/markup/documentation_link.hpp"

namespace standardese::output::markup
{

entity_kind documentation_link::do_get_kind() const noexcept
{
    return entity_kind::documentation_link;
}

std::unique_ptr<entity> documentation_link::do_clone() const
{
    builder b(title(), type_safe::copy(unresolved_destination()).value_or(""));

    if (internal_destination())
        b.peek().resolve_destination(internal_destination().value());
    else if (external_destination())
        b.peek().resolve_destination(external_destination().value());

    for (auto& child : *this)
        b.add_child(detail::unchecked_downcast<phrasing_entity>(child.clone()));

    return b.finish();
}

}

