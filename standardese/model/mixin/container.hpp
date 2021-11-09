// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_MODEL_MIXIN_CONTAINER_HPP_INCLUDED
#define STANDARDESE_MODEL_MIXIN_CONTAINER_HPP_INCLUDED

#include <vector>

#include "../entity.hpp"
#include "../markup/text.hpp"

namespace standardese::model::mixin
{
    /// A base class for entities that are containers.
    /// \tparam T the kinds of entities stored in this container.
    /// Currently, this is almost always just [entity]().
    template <typename T = entity>
    class container
    {
    public:
        using entity = T;
        using iterator = typename std::vector<T>::iterator;
        using const_iterator = typename std::vector<T>::const_iterator;
        using reverse_iterator = typename std::vector<T>::reverse_iterator;
        using const_reverse_iterator = typename std::vector<T>::const_reverse_iterator;

        container() noexcept = default;

        explicit container(std::initializer_list<T> children) : children(std::move(children)) {}

        template <typename ...Args>
        void emplace_back(Args&&... args) {
            children.emplace_back(std::forward<Args>(args)...);
        }

        template <typename S>
        void push_back(S&& child) {
            children.push_back(std::forward<S>(child));
        }

        template <typename S>
        void insert(S&& child) {
            using std::begin;
            children.insert(begin(children), std::forward<S>(child));
        }

        void clear() {
            children.clear();
        }

        void erase(iterator& it)
        {
            children.erase(it);
        }

        /// \returns An iterator to the first child entity.
        iterator begin()
        {
            return children.begin();
        }

        /// \returns An iterator one past the last child entity.
        iterator end()
        {
            return children.end();
        }

        /// \returns An iterator to the last child entity.
        reverse_iterator rbegin()
        {
            return children.rbegin();
        }

        /// \returns An iterator one before the first child entity.
        reverse_iterator rend()
        {
            return children.rend();
        }

        /// \returns An iterator to the first child entity.
        const_iterator begin() const
        {
            return children.begin();
        }

        /// \returns An iterator one past the last child entity.
        const_iterator end() const
        {
            return children.end();
        }

        /// \returns An iterator to the last child entity.
        const_reverse_iterator rbegin() const
        {
            return children.rbegin();
        }

        /// \returns An iterator one before the first child entity.
        const_reverse_iterator rend() const
        {
            return children.rend();
        }

    private:
        // TODO(0.6.0-alpha): Should we just make this public? So we do not need all this
        // glue anymore? Or should we instead expose the entire vector
        // interface here and check NDEBUG that children are of expected
        // types? Such as, lists contain only list items...
        std::vector<T> children;
    };
}

#endif
