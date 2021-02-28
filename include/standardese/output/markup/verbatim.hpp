// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_OUTPUT_MARKUP_VERBATIM_HPP_INCLUDED
#define STANDARDESE_OUTPUT_MARKUP_VERBATIM_HPP_INCLUDED

#include "phrasing_entity.hpp"

namespace standardese::output::markup
{
    /// A fragment that should be excluded in the output as-is.
    class verbatim final : public phrasing_entity
    {
    public:
        /// \returns A new verbatim fragment containing the given string.
        static std::unique_ptr<verbatim> build(std::string str)
        {
            return std::unique_ptr<verbatim>(new verbatim(std::move(str)));
        }

        const std::string& content() const noexcept
        {
            return str_;
        }

    private:
        entity_kind do_get_kind() const noexcept override;

        void do_visit(visitor_callback_t cb, void* mem) const override;

        std::unique_ptr<entity> do_clone() const override;

        explicit verbatim(std::string str) : str_(std::move(str)) {}

        std::string str_;
    };

}

#endif

