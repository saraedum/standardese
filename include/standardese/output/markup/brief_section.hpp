// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//               2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_OUTPUT_MARKUP_BRIEF_SECTION_HPP_INCLUDED
#define STANDARDESE_OUTPUT_MARKUP_BRIEF_SECTION_HPP_INCLUDED

#include "phrasing_entity.hpp"
#include "container_entity.hpp"
#include "block_id.hpp"
#include "doc_section.hpp"

namespace standardese::output::markup
{
    /// The `\brief` section in an entity documentation.
    class brief_section final : public doc_section, public container_entity<phrasing_entity>
    {
    public:
        /// Builds a brief section.
        class builder : public container_builder<brief_section>
        {
        public:
            /// \effects Creates an empty brief section.
            builder() : container_builder(std::unique_ptr<brief_section>(new brief_section())) {}
        };

        /// \returns The unique id of the brief section.
        ///
        /// It is created from the parent id.
        block_id id() const;

    private:
        entity_kind do_get_kind() const noexcept override;

        void do_visit(visitor_callback_t cb, void* mem) const override;

        std::unique_ptr<entity> do_clone() const override;

        brief_section() = default;
    };
}

#endif
