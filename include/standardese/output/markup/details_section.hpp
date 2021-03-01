// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//               2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_OUTPUT_MARKUP_DETAILS_SECTION_HPP_INCLUDED
#define STANDARDESE_OUTPUT_MARKUP_DETAILS_SECTION_HPP_INCLUDED

#include "doc_section.hpp"
#include "container_entity.hpp"

namespace standardese::output::markup
{
    /// A `\details` section in an entity documentation.
    /// \notes There can be multiple `\details` in a documentation, but only one `\brief`.
    class details_section final : public doc_section, public container_entity<block_entity>
    {
    public:
        /// Builds a details section.
        class builder : public container_builder<details_section>
        {
        public:
            /// \effects Creates an empty details section.
            builder() : container_builder(std::unique_ptr<details_section>(new details_section()))
            {}
        };

    private:
        entity_kind do_get_kind() const noexcept override;

        void do_visit(visitor_callback_t cb, void* mem) const override;

        std::unique_ptr<entity> do_clone() const override;

        details_section() = default;
    };
}

#endif
