// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//               2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_OUTPUT_MARKUP_TERM_DESCRIPTION_ITEM_HPP_INCLUDED
#define STANDARDESE_OUTPUT_MARKUP_TERM_DESCRIPTION_ITEM_HPP_INCLUDED

#include "list_item_base.hpp"
#include "term.hpp"
#include "description.hpp"

namespace standardese::output::markup
{
    /// A list item that consists of a term and an description.
    ///
    /// It is a simple key-value pair.
    class term_description_item final : public list_item_base
    {
    public:
        /// \returns A newly built term description.
        static std::unique_ptr<term_description_item> build(
            block_id id, std::unique_ptr<term> term,
            std::unique_ptr<description> desc)
        {
            return std::unique_ptr<term_description_item>(
                new term_description_item(std::move(id), std::move(term), std::move(desc)));
        }

        /// \returns The term.
        const markup::term& term() const noexcept
        {
            return *term_;
        }

        /// \returns The description.
        const markup::description& description() const noexcept
        {
            return *description_;
        }

    private:
        entity_kind do_get_kind() const noexcept override;

        void do_visit(visitor_callback_t cb, void* mem) const override;

        std::unique_ptr<entity> do_clone() const override;

        term_description_item(block_id id, std::unique_ptr<markup::term> t,
                              std::unique_ptr<markup::description> desc)
        : list_item_base(std::move(id)), term_(std::move(t)), description_(std::move(desc))
        {}

        std::unique_ptr<markup::term>        term_;
        std::unique_ptr<markup::description> description_;
    };
}

#endif

