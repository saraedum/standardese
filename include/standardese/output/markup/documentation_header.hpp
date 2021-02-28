// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_OUTPUT_MARKUP_DOCUMENTATION_HEADER_HPP_INCLUDED
#define STANDARDESE_OUTPUT_MARKUP_DOCUMENTATION_HEADER_HPP_INCLUDED

#include <memory>

#include "heading.hpp"

namespace standardese::output::markup
{
    /// The heading in a documentation.
    class documentation_header
    {
    public:
        /// \effects Creates it passing it heading and module.
        documentation_header(std::unique_ptr<markup::heading> h,
                             type_safe::optional<std::string> module = type_safe::nullopt)
        : module_(std::move(module)), heading_(std::move(h))
        {}

        documentation_header clone() const;

        /// \returns A reference to the heading of the documentation.
        /// \group heading
        const markup::heading& heading() const noexcept
        {
            return *heading_;
        }

        /// \group heading
        markup::heading& heading() noexcept
        {
            return *heading_;
        }

        /// \returns The module of the entity, if any.
        const type_safe::optional<std::string>& module() const noexcept
        {
            return module_;
        }

    private:
        type_safe::optional<std::string> module_;
        std::unique_ptr<markup::heading> heading_;
    };
}

#endif
