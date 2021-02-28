// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_OUTPUT_MARKUP_SOFT_BREAK_HPP_INCLUDED
#define STANDARDESE_OUTPUT_MARKUP_SOFT_BREAK_HPP_INCLUDED

#include "phrasing_entity.hpp"

namespace standardese::output::markup
{
    /// A soft line break.
    class soft_break final : public phrasing_entity
    {
    public:
        /// \returns A new soft break.
        static std::unique_ptr<soft_break> build()
        {
            return std::unique_ptr<soft_break>(new soft_break());
        }

    private:
        entity_kind do_get_kind() const noexcept override;

        void do_visit(visitor_callback_t cb, void* mem) const override;

        std::unique_ptr<entity> do_clone() const override;

        soft_break() noexcept = default;
    };
}

#endif

