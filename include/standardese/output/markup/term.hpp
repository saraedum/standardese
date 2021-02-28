// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//               2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_OUTPUT_MARKUP_TERM_HPP_INCLUDED
#define STANDARDESE_OUTPUT_MARKUP_TERM_HPP_INCLUDED

#include "phrasing_entity.hpp"
#include "container_entity.hpp"

namespace standardese::output::markup
{
    /// The term of a [standardese::markup::term_description_list_item]().
    class term final : public phrasing_entity, public container_entity<phrasing_entity>
    {
    public:
        /// Builds a new term.
        class builder : public container_builder<term>
        {
        public:
            /// \effects Creates an empty term.
            builder() : container_builder(std::unique_ptr<term>(new term)) {}
        };

        /// \returns A built term consisting of the single entity.
        static std::unique_ptr<term> build(std::unique_ptr<phrasing_entity> entity)
        {
            return builder().add_child(std::move(entity)).finish();
        }

    private:
        entity_kind do_get_kind() const noexcept override;

        void do_visit(visitor_callback_t cb, void* mem) const override;

        std::unique_ptr<entity> do_clone() const override;

        term() = default;
    };
}

#endif

