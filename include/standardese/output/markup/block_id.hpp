// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_OUTPUT_MARKUP_BLOCK_ID_HPP_INCLUDED
#define STANDARDESE_OUTPUT_MARKUP_BLOCK_ID_HPP_INCLUDED

#include <string>

namespace standardese::output::markup
{
    /// The id of a [standardese::markup::block_entity]().
    ///
    /// It must be unique and should only consist of alphanumerics or `-`.
    class block_id
    {
    public:
        /// \effects Creates an empty id.
        explicit block_id() : block_id("") {}

        /// \effects Creates it given the string representation.
        explicit block_id(std::string id) : id_(std::move(id)) {}

        /// \returns Whether or not the id is empty.
        bool empty() const noexcept
        {
            return id_.empty();
        }

        /// \returns The string representation of the id.
        const std::string& as_str() const noexcept
        {
            return id_;
        }

        /// \returns The escaped string representaton.
        std::string as_output_str() const;

    private:
        std::string id_;
    };

    /// \returns Whether or not two ids are (un-)equal.
    /// \group block_id_equal block_id comparison
    inline bool operator==(const block_id& a, const block_id& b) noexcept
    {
        return a.as_str() == b.as_str();
    }

    /// \group block_id_equal
    inline bool operator!=(const block_id& a, const block_id& b) noexcept
    {
        return !(a == b);
    }
}

#endif
