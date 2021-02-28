// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_OUTPUT_MARKUP_ENTITY_INDEX_ITEM_HPP_INCLUDED
#define STANDARDESE_OUTPUT_MARKUP_ENTITY_INDEX_ITEM_HPP_INCLUDED

#include "list_item_base.hpp"
#include "term.hpp"
#include "description.hpp"

namespace standardese::output::markup
{
    /// A list item that is an entity of the [standardese::entity_index]().
    /// \notes This can also be used for the file index and namespace index.
    class entity_index_item final : public list_item_base
    {
    public:
        /// \returns A newly built entity index item.
        static std::unique_ptr<entity_index_item> build(block_id id, std::unique_ptr<term> entity,
                                                        std::unique_ptr<description> brief
                                                        = nullptr)
        {
            return std::unique_ptr<entity_index_item>(
                new entity_index_item(std::move(id), std::move(entity), std::move(brief)));
        }

        /// \returns The entity name.
        const term& entity() const noexcept
        {
            return *entity_;
        }

        /// \returns The brief description of the entity, if it has any.
        type_safe::optional_ref<const description> brief() const noexcept
        {
            return type_safe::opt_ref(brief_.get());
        }

    private:
        entity_index_item(block_id id, std::unique_ptr<term> entity,
                          std::unique_ptr<description> brief)
        : list_item_base(std::move(id)), entity_(std::move(entity)), brief_(std::move(brief))
        {}

        entity_kind do_get_kind() const noexcept override;

        void do_visit(visitor_callback_t cb, void* mem) const override;

        std::unique_ptr<markup::entity> do_clone() const override;

        std::unique_ptr<term>        entity_;
        std::unique_ptr<description> brief_;
    };
}

#endif

