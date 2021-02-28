// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_OUTPUT_MARKUP_ENTITY_HPP_INCLUDED
#define STANDARDESE_OUTPUT_MARKUP_ENTITY_HPP_INCLUDED

#include <memory>
#include <type_safe/optional_ref.hpp>

#include "entity_kind.hpp"

namespace standardese::output::markup
{
    /// The base class for all markup entities.
    ///
    /// This is how the documentation output is described.
    /// An entity can be converted into various markup languages.
    class entity
    {
    public:
        entity(const entity&) = delete;
        entity& operator=(const entity&) = delete;
        virtual ~entity() noexcept       = default;

        /// \returns The kind of entity.
        entity_kind kind() const noexcept
        {
            return do_get_kind();
        }

        /// \returns A reference to the parent entity, if there is any.
        type_safe::optional_ref<const entity> parent() const noexcept
        {
            return parent_;
        }

        /// \returns A copy of itself.
        std::unique_ptr<entity> clone() const
        {
            return do_clone();
        }

        template <typename Func>
        static void visit(const entity& e, Func f) {
            // TODO
            throw std::logic_error("TODO: visit");
        }

    protected:
        entity() noexcept = default;

        using visitor_callback_t = void (*)(void* mem, const entity&);

        /// \effects Sets the parent of `child` to `*this`.
        void set_ownership(entity& child) const
        {
            child.parent_ = type_safe::ref(*this);
        }

    private:
        /// \returns The kind of entity.
        virtual entity_kind do_get_kind() const noexcept = 0;

        /// \effects Invokes the callback for all children.
        virtual void do_visit(visitor_callback_t cb, void* mem) const = 0;

        /// \returns A copy of itself.
        virtual std::unique_ptr<entity> do_clone() const = 0;

        type_safe::optional_ref<const entity> parent_;
    };

    /// \exclude
    namespace detail
    {
        template <typename U, typename T>
        std::unique_ptr<U> unchecked_downcast(std::unique_ptr<T> ptr)
        {
            return std::unique_ptr<U>(static_cast<U*>(ptr.release()));
        }
    } // namespace detail

    /// \returns A copy of the given entity.
    /// \requires `T` must be derived from [standardese::markup::entity]().
    template <typename T>
    std::unique_ptr<T> clone(const T& obj)
    {
        static_assert(std::is_base_of<entity, T>::value, "must be derived from entity");
        return std::unique_ptr<T>(static_cast<T*>(obj.clone().release()));
    }

    /// \exclude
    namespace detail
    {
    } // namespace detail

}

#endif // STANDARDESE_OUTPUT_MARKUP_ENTITY_HPP_INCLUDED
