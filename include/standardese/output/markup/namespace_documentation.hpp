// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_OUTPUT_MARKUP_NAMESPACE_DOCUMENTATION_HPP_INCLUDED
#define STANDARDESE_OUTPUT_MARKUP_NAMESPACE_DOCUMENTATION_HPP_INCLUDED

#include <cppast/forward.hpp>

#include "documentation_entity.hpp"
#include "entity_index_item.hpp"

namespace standardese::output::markup
{
    /// The documentation of a namespace.
    ///
    /// It is meant to be used in the entity index.
    class namespace_documentation final : public documentation_entity,
                                          public container_entity<block_entity>
    {
    public:
        /// Builds the documentation of a namespace.
        class builder : public documentation_builder<container_builder<namespace_documentation>>
        {
        public:
            /// \effects Creates it giving the id and header.
            /// \requires The user data of the namespace must either be `nullptr` or the
            /// corresponding [standardese::doc_entity]().
            builder(type_safe::object_ref<const cppast::cpp_namespace> ns, block_id id,
                    type_safe::optional<documentation_header> h)
            : documentation_builder(std::unique_ptr<namespace_documentation>(
                  new namespace_documentation(ns, std::move(id), std::move(h))))
            {}

            builder& add_child(std::unique_ptr<entity_index_item> entity)
            {
                container_builder::add_child(std::move(entity));
                return *this;
            }

            builder& add_child(std::unique_ptr<namespace_documentation> entity)
            {
                container_builder::add_child(std::move(entity));
                return *this;
            }

        private:
            using container_builder::add_child;
        };

        const cppast::cpp_namespace& namespace_() const noexcept
        {
            return *ns_;
        }

    private:
        namespace_documentation(type_safe::object_ref<const cppast::cpp_namespace> ns, block_id id,
                                type_safe::optional<documentation_header> h)
        : documentation_entity(std::move(id), std::move(h), nullptr), ns_(ns)
        {}

        entity_kind do_get_kind() const noexcept override;

        void do_visit(visitor_callback_t cb, void* mem) const override;

        std::unique_ptr<entity> do_clone() const override;

        type_safe::object_ref<const cppast::cpp_namespace> ns_;
    };
}

#endif

