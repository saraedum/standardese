// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_OUTPUT_MARKUP_DOCUMENT_ENTITY_HPP_INCLUDED
#define STANDARDESE_OUTPUT_MARKUP_DOCUMENT_ENTITY_HPP_INCLUDED

#include "block_entity.hpp"
#include "container_entity.hpp"
#include "output_name.hpp"

namespace standardese::output::markup
{
    /// Base class for entities representing a stand-alone document.
    ///
    /// Those are the root nodes of the markup AST.
    class document_entity : public entity, public container_entity<block_entity>
    {
    public:
        /// \returns The title of the document.
        const std::string& title() const noexcept
        {
            return title_;
        }

        /// \returns The output name of the document.
        const markup::output_name& output_name() const noexcept
        {
            return output_name_;
        }

    protected:
        document_entity(std::string title, markup::output_name name)
        : output_name_(std::move(name)), title_(std::move(title))
        {}

    private:
        void do_visit(visitor_callback_t cb, void* mem) const override;

        markup::output_name output_name_;
        std::string         title_;
    };
}

#endif

