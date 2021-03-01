// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_OUTPUT_MARKUP_OUTPUT_NAME_HPP_INCLUDED
#define STANDARDESE_OUTPUT_MARKUP_OUTPUT_NAME_HPP_INCLUDED

#include <string>

#include "../../forward.hpp"

namespace standardese::output::markup
{
    /// The file name of a [standardese::markup::document_entity]().
    ///
    /// It can either contain the extension already or need one.
    class output_name
    {
    public:
        /// \returns An `output_name` that still needs an extension.
        static output_name from_name(std::string name)
        {
            return output_name(std::move(name), true);
        }

        /// \returns An `output_name` that already has an extension.
        static output_name from_file_name(std::string file_name)
        {
            return output_name(std::move(file_name), false);
        }

        /// \returns The name of the file, may or may not contain the extension.
        const std::string& name() const noexcept
        {
            return name_;
        }

        /// \returns Whether or not it needs an extension.
        bool needs_extension() const noexcept
        {
            return needs_extension_;
        }

        /// \returns The file name of the output name, given the extension of the current format.
        std::string file_name(const char* format_extension) const
        {
            return needs_extension_ ? name() + "." + format_extension : name();
        }

    private:
        output_name(std::string name, bool need) : name_(std::move(name)), needs_extension_(need) {}

        std::string name_;
        bool        needs_extension_;
    };
}

#endif
