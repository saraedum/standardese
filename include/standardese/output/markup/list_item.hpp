// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//               2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_OUTPUT_MARKUP_LIST_ITEM_HPP_INCLUDED
#define STANDARDESE_OUTPUT_MARKUP_LIST_ITEM_HPP_INCLUDED

#include "list_item_base.hpp"
#include "container_entity.hpp"

namespace standardese::output::markup
{
    /// An item in a list.
    ///
    /// This is the HTML `<li>` container.
    class list_item final : public list_item_base, public container_entity<block_entity>
    {
    public:
        /// Builds a list item.
        class builder : public container_builder<list_item>
        {
        public:
            /// \effects Builds an empty item.
            builder(block_id id = block_id())
            : container_builder(std::unique_ptr<list_item>(new list_item(std::move(id))))
            {}
        };

        /// \returns A list item consisting of the given block only.
        static std::unique_ptr<list_item> build(std::unique_ptr<block_entity> block)
        {
            return builder().add_child(std::move(block)).finish();
        }

    private:
        entity_kind do_get_kind() const noexcept override;

        void do_visit(visitor_callback_t cb, void* mem) const override;

        std::unique_ptr<entity> do_clone() const override;

        list_item(block_id id) : list_item_base(std::move(id)) {}
    };
}

#endif
