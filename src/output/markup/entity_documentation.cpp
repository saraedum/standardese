// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include "../include/standardese/output/markup/entity_documentation.hpp"

namespace standardese::output::markup
{

entity_kind entity_documentation::do_get_kind() const noexcept
{
    return entity_kind::entity_documentation;
}

void entity_documentation::do_visit(visitor_callback_t cb, void* mem) const
{
    if (header())
        cb(mem, header().value().heading());
    if (synopsis())
        cb(mem, synopsis().value());

    for (auto& sec : doc_sections())
        cb(mem, sec);

    for (auto& child : *this)
        cb(mem, child);
}

std::unique_ptr<entity> entity_documentation::do_clone() const
{
    builder b(entity_, id(),
              header() ? type_safe::make_optional(header().value().clone()) : type_safe::nullopt,
              synopsis() ? markup::clone(synopsis().value()) : nullptr);
    for (auto& sec : doc_sections())
        b.add_section_impl(detail::unchecked_downcast<doc_section>(sec.clone()));
    for (auto& child : *this)
        b.add_child(detail::unchecked_downcast<entity_documentation>(child.clone()));
    return b.finish();
}

}

