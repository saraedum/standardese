// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_OUTPUT_MARKUP_DETAIL_VECTOR_PTR_RANGE_HPP_INCLUDED
#define STANDARDESE_OUTPUT_MARKUP_DETAIL_VECTOR_PTR_RANGE_HPP_INCLUDED

#include "vector_ptr_iterator.hpp"

namespace standardese::output::markup::detail
{
        template <typename T>
        struct vector_ptr_range
        {
            vector_ptr_iterator<T> begin_, end_;

            const vector_ptr_iterator<T>& begin() const noexcept
            {
                return begin_;
            }

            const vector_ptr_iterator<T>& end() const noexcept
            {
                return end_;
            }
        };
}

#endif
