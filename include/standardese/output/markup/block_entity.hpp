// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_OUTPUT_MARKUP_BLOCK_ENTITY_HPP_INCLUDED
#define STANDARDESE_OUTPUT_MARKUP_BLOCK_ENTITY_HPP_INCLUDED

#include "block_id.hpp"
#include "entity.hpp"

namespace standardese::output::markup
{
    /// Base class for all block entities.
    ///
    /// A block entity is part of the structure of the document.
    class block_entity : public entity
    {
    public:
        /// \returns The unique id of the block.
        const block_id& id() const noexcept
        {
            return id_;
        }

    protected:
        /// \effects Creates it giving the id.
        block_entity(block_id id) : id_(std::move(id)) {}

    private:
        block_id id_;
    };
}

#endif
