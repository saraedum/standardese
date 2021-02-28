// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_OUTPUT_MARKUP_LINK_BASE_HPP_INCLUDED
#define STANDARDESE_OUTPUT_MARKUP_LINK_BASE_HPP_INCLUDED

#include "phrasing_entity.hpp"
#include "container_entity.hpp"

namespace standardese::output::markup
{
    /// Base class for all links.
    ///
    /// The actual link text will be stored as the children.
    class link_base : public phrasing_entity, public container_entity<phrasing_entity>
    {
    public:
        /// \returns The title of the link.
        const std::string& title() const noexcept
        {
            return title_;
        }

    protected:
        /// \effects Sets the title of the link.
        link_base(std::string title) : title_(std::move(title)) {}

    private:
        void do_visit(visitor_callback_t cb, void* mem) const override;

        std::string title_;
    };
}

#endif

