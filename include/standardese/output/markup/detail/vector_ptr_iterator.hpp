// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_OUTPUT_MARKUP_DETAIL_VECTOR_PTR_ITERATOR_HPP_INCLUDED
#define STANDARDESE_OUTPUT_MARKUP_DETAIL_VECTOR_PTR_ITERATOR_HPP_INCLUDED

#include <vector>
#include <memory>
#include <type_safe/reference.hpp>

#include "../entity.hpp"

namespace standardese::output::markup::detail
{

        template <typename T>
        class vector_ptr_iterator
        {
            using container = std::vector<std::unique_ptr<T>>;

        public:
            using value_type        = const T;
            using reference         = const T&;
            using pointer           = const T*;
            using difference_type   = std::ptrdiff_t;
            using iterator_category = std::bidirectional_iterator_tag;

            vector_ptr_iterator() noexcept : cur_(nullptr) {}

            vector_ptr_iterator(typename container::const_iterator cur) : cur_(cur) {}

            reference operator*() const noexcept
            {
                return **cur_;
            }

            pointer operator->() const noexcept
            {
                return cur_->get();
            }

            vector_ptr_iterator& operator++() noexcept
            {
                ++cur_;
                return *this;
            }

            vector_ptr_iterator operator++(int) noexcept
            {
                auto tmp = *this;
                ++(*this);
                return tmp;
            }

            vector_ptr_iterator& operator--() noexcept
            {
                --cur_;
                return *this;
            }

            vector_ptr_iterator operator--(int) noexcept
            {
                auto tmp = *this;
                --(*this);
                return tmp;
            }

            friend bool operator==(const vector_ptr_iterator& a,
                                   const vector_ptr_iterator& b) noexcept
            {
                return a.cur_ == b.cur_;
            }

            friend bool operator!=(const vector_ptr_iterator& a,
                                   const vector_ptr_iterator& b) noexcept
            {
                return !(a == b);
            }

        private:
            typename container::const_iterator cur_;
        };
}

#endif
