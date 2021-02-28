// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_OUTPUT_MARKUP_TEXT_HPP_INCLUDED
#define STANDARDESE_OUTPUT_MARKUP_TEXT_HPP_INCLUDED

#include "phrasing_entity.hpp"

namespace standardese::output::markup
{
    /// A normal text fragment.
    class text final : public phrasing_entity
    {
    public:
        /// \returns A new text fragment containing the given text.
        static std::unique_ptr<text> build(std::string t)
        {
            return std::unique_ptr<text>(new text(std::move(t)));
        }

        /// \returns The text of the text fragment.
        const std::string& string() const noexcept
        {
            return text_;
        }

    private:
        entity_kind do_get_kind() const noexcept override;

        void do_visit(visitor_callback_t cb, void* mem) const override;

        std::unique_ptr<entity> do_clone() const override;

        text(std::string text) : text_(std::move(text)) {}

        std::string text_;
    };
}

#endif

