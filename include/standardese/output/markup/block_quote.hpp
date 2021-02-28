// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_OUTPUT_MARKUP_BLOCK_QUOTE_HPP_INCLUDED
#define STANDARDESE_OUTPUT_MARKUP_BLOCK_QUOTE_HPP_INCLUDED

#include "block_entity.hpp"
#include "container_entity.hpp"

namespace standardese::output::markup
{
    /// A block quote.
    class block_quote final : public block_entity, public container_entity<block_entity>
    {
    public:
        /// Builds a new quote.
        class builder : public container_builder<block_quote>
        {
        public:
            /// \effects Creates an empty quote.
            builder(block_id id)
            : container_builder(std::unique_ptr<block_quote>(new block_quote(std::move(id))))
            {}
        };

    private:
        entity_kind do_get_kind() const noexcept override;

        void do_visit(visitor_callback_t cb, void* mem) const override;

        std::unique_ptr<entity> do_clone() const override;

        block_quote(block_id id) : block_entity(std::move(id)) {}
    };
}

#endif

