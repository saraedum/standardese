// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_OUTPUT_MARKUP_CONTAINER_ENTITY_HPP_INCLUDED
#define STANDARDESE_OUTPUT_MARKUP_CONTAINER_ENTITY_HPP_INCLUDED

#include <vector>
#include <memory>

#include "detail/vector_ptr_iterator.hpp"
#include "detail/parent_updater.hpp"

namespace standardese::output::markup
{

    /// A mix-in base class for entity that are a containers.
    ///
    /// It takes care of the container functions.
    /// \requires `T` must be derived from `entity`.
    template <typename T>
    class container_entity
    {
        using container = std::vector<std::unique_ptr<T>>;

    public:
        using iterator = detail::vector_ptr_iterator<T>;

        /// \returns An iterator to the first child entity.
        iterator begin() const noexcept
        {
            return children_.begin();
        }

        /// \returns An iterator one past the last child entity.
        iterator end() const noexcept
        {
            return children_.end();
        }

    protected:
        ~container_entity() noexcept
        {
            static_assert(std::is_base_of<entity, T>::value, "T must be derived from entity");
        }

        /// Base class to create the builder.
        /// \requires `Derived` is the type derived from `entity_container`.
        template <typename Derived>
        class container_builder
        {
        public:
            /// \effects Adds a new child for the container.
            container_builder& add_child(std::unique_ptr<T> entity)
            {
                if (entity)
                {
                    detail::parent_updater::set(*entity, type_safe::ref(*result_));
                    as_container().children_.push_back(std::move(entity));
                }
                return *this;
            }

            /// \returns Whether or not the container is empty.
            bool empty() noexcept
            {
                return result_->begin() == result_->end();
            }

            /// \returns The finished entity.
            std::unique_ptr<Derived> finish() noexcept
            {
                return std::move(result_);
            }

        protected:
            /// \effects Creates it giving it the partially constructed entity.
            container_builder(std::unique_ptr<Derived> entity) noexcept : result_(std::move(entity))
            {
                static_assert(std::is_base_of<container_entity<T>, Derived>::value,
                              "Derived must be derived from container_entity");
            }

            /// \returns The not yet finished entity.
            /// \group peek
            Derived& peek() noexcept
            {
                return *result_;
            }

            /// \group peek
            const Derived& peek() const noexcept
            {
                return *result_;
            }

        private:
            container_entity<T>& as_container() const noexcept
            {
                return *result_;
            }

            std::unique_ptr<Derived> result_;
        };

    private:
        container children_;
    };

}

#endif
