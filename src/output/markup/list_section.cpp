// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include "../../../include/standardese/output/markup/list_section.hpp"

namespace standardese::output::markup
{

entity_kind list_section::do_get_kind() const noexcept
{
    return entity_kind::list_section;
}

std::unique_ptr<list_section> list_section::build(std::string name,
                                                  std::unique_ptr<unordered_list> list)
{
    return std::unique_ptr<list_section>(new list_section(std::move(name), std::move(list)));
}

list_section::list_section(std::string name,
                           std::unique_ptr<unordered_list> list)
: name_(std::move(name)), list_(std::move(list))
{}

void list_section::do_visit(visitor_callback_t cb, void* mem) const
{
    for (auto& child : *this)
        cb(mem, child);
}

std::unique_ptr<entity> list_section::do_clone() const
{
    return build(name(), detail::unchecked_downcast<unordered_list>(list_->clone()));
}

}
