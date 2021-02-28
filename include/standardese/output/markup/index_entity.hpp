// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_OUTPUT_MARKUP_INDEX_ENTITY_HPP_INCLUDED
#define STANDARDESE_OUTPUT_MARKUP_INDEX_ENTITY_HPP_INCLUDED

#include "block_entity.hpp"
#include "heading.hpp"

namespace standardese::output::markup
{
    /// Base class for all kinds of documentation indices.
    class index_entity : public block_entity
    {
    public:
        /// \returns The heading of the index.
        const markup::heading& heading() const noexcept
        {
            return *heading_;
        }

    protected:
        index_entity(block_id id, std::unique_ptr<markup::heading> h)
        : block_entity(std::move(id)), heading_(std::move(h))
        {}

    private:
        std::unique_ptr<markup::heading> heading_;
    };
}

#endif

