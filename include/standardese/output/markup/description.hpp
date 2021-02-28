// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//               2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_OUTPUT_MARKUP_DESCRIPTION_HPP_INCLUDED
#define STANDARDESE_OUTPUT_MARKUP_DESCRIPTION_HPP_INCLUDED

#include "phrasing_entity.hpp"
#include "container_entity.hpp"

namespace standardese::output::markup
{
    /// The description of a [standardese::markup::term_description_list_item]().
    class description final : public phrasing_entity, public container_entity<phrasing_entity>
    {
    public:
        /// Builds a description.
        class builder : public container_builder<description>
        {
        public:
            /// \effects Creates an empty description.
            builder() : container_builder(std::unique_ptr<description>(new description)) {}
        };

        /// \returns A newly built description consisting only of the given phrasing entity.
        static std::unique_ptr<description> build(std::unique_ptr<phrasing_entity> phrasing)
        {
            return builder().add_child(std::move(phrasing)).finish();
        }

    private:
        entity_kind do_get_kind() const noexcept override;

        void do_visit(visitor_callback_t cb, void* mem) const override;

        std::unique_ptr<entity> do_clone() const override;

        description() = default;
    };
}

#endif

