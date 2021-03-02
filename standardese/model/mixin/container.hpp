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
    template <typename T = entity>
    class container
    {
        // TODO: Maybe this sugar is not worth it. At least not outside of the constructor.
        model::markup::text convert(std::string text) {
            return model::markup::text(std::move(text)); 
        }

        model::markup::text convert(const char* text) {
            return model::markup::text(std::move(text)); 
        }

        template <typename S>
        T convert(S&& s) {
          return std::forward<S>(s);
        }

    public:
        using entity = T;
        using iterator = typename std::vector<T>::iterator;
        using const_iterator = typename std::vector<T>::const_iterator;
        using reverse_iterator = typename std::vector<T>::reverse_iterator;
        using const_reverse_iterator = typename std::vector<T>::const_reverse_iterator;

        container() noexcept = default;

        template <typename ...Args>
        explicit container(Args&&... args) : children_{convert(std::forward<Args>(args))...} {}

        // TODO: Rename to emplace_back()
        template <typename ...Args>
        void emplace_child(Args&&... args) {
            children_.emplace_back(std::forward<Args>(args)...);
        }

        // TODO: Rename to push_back()
        template <typename S>
        void add_child(S&& child) {
            children_.push_back(convert(std::forward<S>(child)));
        }

        template <typename S>
        void insert_child(S&& child) {
            using std::begin;
            children_.insert(begin(children_), convert(std::forward<S>(child)));
        }

        void clear() {
            children_.clear();
        }

        /// \returns An iterator to the first child entity.
        iterator begin()
        {
            return children_.begin();
        }

        /// \returns An iterator one past the last child entity.
        iterator end()
        {
            return children_.end();
        }

        /// \returns An iterator to the last child entity.
        reverse_iterator rbegin()
        {
            return children_.rbegin();
        }

        /// \returns An iterator one before the first child entity.
        reverse_iterator rend()
        {
            return children_.rend();
        }

        /// \returns An iterator to the first child entity.
        const_iterator begin() const
        {
            return children_.begin();
        }

        /// \returns An iterator one past the last child entity.
        const_iterator end() const
        {
            return children_.end();
        }

        /// \returns An iterator to the last child entity.
        const_reverse_iterator rbegin() const
        {
            return children_.rbegin();
        }

        /// \returns An iterator one before the first child entity.
        const_reverse_iterator rend() const
        {
            return children_.rend();
        }

    private:
        // TODO: Should we just make this public? So we do not need all this glue anymore?
        std::vector<T> children_;
    };
}

#endif
