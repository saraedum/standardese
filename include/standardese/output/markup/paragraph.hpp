// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_OUTPUT_MARKUP_PARAGRAPH_HPP_INCLUDED
#define STANDARDESE_OUTPUT_MARKUP_PARAGRAPH_HPP_INCLUDED

#include "block_entity.hpp"
#include "phrasing_entity.hpp"
#include "container_entity.hpp"

namespace standardese::output::markup
{
    /// A plain paragraph.
    class paragraph final : public block_entity, public container_entity<phrasing_entity>
    {
    public:
        /// Builds a paragraph.
        class builder : public container_builder<paragraph>
        {
        public:
            /// \effects Creates it giving the id.
            builder(block_id id = block_id())
            : container_builder(std::unique_ptr<paragraph>(new paragraph(std::move(id))))
            {}
        };

    private:
        entity_kind do_get_kind() const noexcept override;

        void do_visit(visitor_callback_t cb, void* mem) const override;

        std::unique_ptr<entity> do_clone() const override;

        paragraph(block_id id) : block_entity(std::move(id)) {}
    };
}

#endif
