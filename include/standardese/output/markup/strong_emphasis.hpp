// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_OUTPUT_MARKUP_STRONG_EMPHASIS_HPP_INCLUDED
#define STANDARDESE_OUTPUT_MARKUP_STRONG_EMPHASIS_HPP_INCLUDED

#include "phrasing_entity.hpp"
#include "container_entity.hpp"
#include "text.hpp"

namespace standardese::output::markup
{
    /// A fragment that is strongly emphasized.
    ///
    /// This corresponds to the `<strong>` HTML tag.
    class strong_emphasis final : public phrasing_entity, public container_entity<phrasing_entity>
    {
    public:
        /// Builds a strong emphasis entity.
        class builder : public container_builder<strong_emphasis>
        {
        public:
            /// \effects Creates it without any children.
            builder() : container_builder(std::unique_ptr<strong_emphasis>(new strong_emphasis()))
            {}
        };

        /// \returns A new strongly emphasized text fragment.
        static std::unique_ptr<strong_emphasis> build(std::string t)
        {
            builder b;
            b.add_child(text::build(std::move(t)));
            return b.finish();
        }

    private:
        entity_kind do_get_kind() const noexcept override;

        void do_visit(visitor_callback_t cb, void* mem) const override;

        std::unique_ptr<entity> do_clone() const override;

        strong_emphasis() = default;
    };

}

#endif

