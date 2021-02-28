// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//               2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_OUTPUT_MARKUP_LIST_SECTION_HPP_INCLUDED
#define STANDARDESE_OUTPUT_MARKUP_LIST_SECTION_HPP_INCLUDED

#include "doc_section.hpp"
#include "block_id.hpp"
#include "unordered_list.hpp"

namespace standardese::output::markup
{
    /// A section containing a list.
    ///
    /// For example this is used to render the list of parameters to a function or method.
    class list_section final : public doc_section
    {
    public:
        /// \returns A newly built list section.
        static std::unique_ptr<list_section> build(std::string name,
                                                   std::unique_ptr<unordered_list> list);

        /// \returns The name of the section.
        const std::string& name() const noexcept
        {
            return name_;
        }

        /// \returns The id of the section.
        const block_id& id() const noexcept
        {
            return list_->id();
        }

        using iterator = unordered_list::iterator;

        /// \returns An iterator to the first list item.
        iterator begin() const noexcept
        {
            return list_->begin();
        }

        /// \returns An iterator one past the last item.
        iterator end() const noexcept
        {
            return list_->end();
        }

    private:
        entity_kind do_get_kind() const noexcept override;

        void do_visit(visitor_callback_t cb, void* mem) const override;

        std::unique_ptr<entity> do_clone() const override;

        list_section(std::string name, std::unique_ptr<unordered_list> list);

        std::string                     name_;
        std::unique_ptr<unordered_list> list_;
    };
}

#endif

