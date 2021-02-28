// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include "../../../include/standardese/output/markup/verbatim.hpp"

namespace standardese::output::markup
{

entity_kind verbatim::do_get_kind() const noexcept
{
    return entity_kind::verbatim;
}

void verbatim::do_visit(visitor_callback_t, void*) const {}

std::unique_ptr<entity> verbatim::do_clone() const
{
    return build(str_);
}

}
