// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_OUTPUT_MARKUP_ENTITY_INDEX_HPP_INCLUDED
#define STANDARDESE_OUTPUT_MARKUP_ENTITY_INDEX_HPP_INCLUDED

#include "index_entity.hpp"
#include "block_entity.hpp"
#include "entity_index_item.hpp"
#include "namespace_documentation.hpp"

namespace standardese::output::markup
{
    /// The index of all entities.
    /// \notes This is created by [standardese::entity_index]().
    class entity_index final : public index_entity, public container_entity<block_entity>
    {
    public:
        /// Builds the entity index.
        class builder : public container_builder<entity_index>
        {
        public:
            /// \effects Creates it given the heading.
            builder(std::unique_ptr<markup::heading> h)
            : container_builder(std::unique_ptr<entity_index>(new entity_index(std::move(h))))
            {}

            builder& add_child(std::unique_ptr<entity_index_item> item)
            {
                container_builder::add_child(std::move(item));
                return *this;
            }

            builder& add_child(std::unique_ptr<namespace_documentation> ns)
            {
                container_builder::add_child(std::move(ns));
                return *this;
            }

        private:
            using container_builder::add_child;
        };

    private:
        entity_index(std::unique_ptr<markup::heading> h)
        : index_entity(block_id("entity-index"), std::move(h))
        {}

        entity_kind do_get_kind() const noexcept override;

        void do_visit(visitor_callback_t cb, void* mem) const override;

        std::unique_ptr<entity> do_clone() const override;
    };
}

#endif

