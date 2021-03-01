// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_OUTPUT_MARKUP_URL_HPP_INCLUDED
#define STANDARDESE_OUTPUT_MARKUP_URL_HPP_INCLUDED

#include <string>

#include "../../forward.hpp"

namespace standardese::output::markup
{
    /// A URL.
    class url
    {
    public:
        explicit url(std::string url) : str_(std::move(url)) {}

        const std::string& as_str() const noexcept
        {
            return str_;
        }

    private:
        std::string str_;
    };
}

#endif
