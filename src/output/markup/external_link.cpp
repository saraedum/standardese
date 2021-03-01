// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include "../../../include/standardese/output/markup/external_link.hpp"

namespace standardese::output::markup
{

entity_kind external_link::do_get_kind() const noexcept
{
    return entity_kind::external_link;
}

std::unique_ptr<entity> external_link::do_clone() const
{
    builder b(title(), url());
    for (auto& child : *this)
        b.add_child(detail::unchecked_downcast<phrasing_entity>(child.clone()));
    return b.finish();
}

}
