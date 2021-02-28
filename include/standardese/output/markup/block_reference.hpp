// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_OUTPUT_MARKUP_BLOCK_REFERENCE_HPP_INCLUDED
#define STANDARDESE_OUTPUT_MARKUP_BLOCK_REFERENCE_HPP_INCLUDED

#include "output_name.hpp"
#include "block_id.hpp"

#include <type_safe/optional.hpp>

namespace standardese::output::markup {

    /// A reference to a block specified by output name and id.
    ///
    /// It is used as the target of [standardese::markup::documentation_link](),
    /// for example.
    class block_reference
    {
    public:
        /// \effects Creates it giving output name and id.
        block_reference(output_name document, block_id id)
        : document_(std::move(document)), id_(std::move(id))
        {}

        /// \effects Creates it giving id only,
        /// the block is then in the same file as the entity that stores the reference.
        block_reference(block_id id) : id_(std::move(id)) {}

        /// \returns The output name of the document the block is in.
        /// If it does not have a document, the block is in the same document.
        const type_safe::optional<output_name>& document() const noexcept
        {
            return document_;
        }

        /// \returns The id of the block.
        const block_id& id() const noexcept
        {
            return id_;
        }

    private:
        type_safe::optional<output_name> document_;
        block_id                         id_;
    };

}

#endif
