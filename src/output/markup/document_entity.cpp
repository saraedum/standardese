// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include "../../../include/standardese/output/markup/document_entity.hpp"

namespace standardese::output::markup
{

void document_entity::do_visit(visitor_callback_t cb, void* mem) const
{
    for (auto& child : *this)
        cb(mem, child);
}

}
