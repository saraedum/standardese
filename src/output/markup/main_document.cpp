// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include "../../../include/standardese/output/markup/main_document.hpp"

namespace standardese::output::markup
{

entity_kind main_document::do_get_kind() const noexcept
{
    return entity_kind::main_document;
}

std::unique_ptr<entity> main_document::do_clone() const
{
    builder b(title(), output_name().name());
    for (auto& child : *this)
        b.add_child(detail::unchecked_downcast<block_entity>(child.clone()));
    return b.finish();
}

}
