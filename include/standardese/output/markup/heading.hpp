// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_OUTPUT_MARKUP_HEADING_HPP_INCLUDED
#define STANDARDESE_OUTPUT_MARKUP_HEADING_HPP_INCLUDED

#include "block_entity.hpp"
#include "phrasing_entity.hpp"
#include "container_entity.hpp"
#include "text.hpp"

namespace standardese::output::markup
{
    /// A heading.
    ///
    /// By default, this corresponds to LaTeX `\paragraph` or HTML level 4,
    /// but this can change, depending on the context it is in.
    class heading final : public block_entity, public container_entity<phrasing_entity>
    {
    public:
        /// Builds a heading.
        class builder : public container_builder<heading>
        {
        public:
            /// \effects Creates an empty heading given its id.
            builder(block_id id)
            : container_builder(std::unique_ptr<heading>(new heading(std::move(id))))
            {}
        };

        /// Builds a heading containing the given string as text.
        static std::unique_ptr<heading> build(block_id id, std::string text)
        {
            return builder(id).add_child(text::build(std::move(text))).finish();
        }

    private:
        entity_kind do_get_kind() const noexcept override;

        void do_visit(visitor_callback_t cb, void* mem) const override;

        std::unique_ptr<entity> do_clone() const override;

        heading(block_id id) : block_entity(std::move(id)) {}
    };

}

#endif
