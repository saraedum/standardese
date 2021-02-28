// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//               2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_OUTPUT_MARKUP_ORDERED_LIST_HPP_INCLUDED
#define STANDARDESE_OUTPUT_MARKUP_ORDERED_LIST_HPP_INCLUDED

#include "block_entity.hpp"
#include "list_item_base.hpp"
#include "container_entity.hpp"

namespace standardese::output::markup
{
    /// An ordered list of items.
    class ordered_list final : public block_entity, public container_entity<list_item_base>
    {
    public:
        /// Builds an ordered list.
        class builder : public container_builder<ordered_list>
        {
        public:
            /// \effects Builds an empty list.
            builder(block_id id)
            : container_builder(std::unique_ptr<ordered_list>(new ordered_list(std::move(id))))
            {}

            /// \effects Adds a new item.
            builder& add_item(std::unique_ptr<list_item_base> item)
            {
                add_child(std::move(item));
                return *this;
            }

        private:
            using container_builder::add_child;
        };

    private:
        entity_kind do_get_kind() const noexcept override;

        void do_visit(visitor_callback_t cb, void* mem) const override;

        std::unique_ptr<entity> do_clone() const override;

        ordered_list(block_id id) : block_entity(std::move(id)) {}
    };
}

#endif
