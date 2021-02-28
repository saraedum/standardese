// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_OUTPUT_MARKUP_ENTITY_DOCUMENTATION_HPP_INCLUDED
#define STANDARDESE_OUTPUT_MARKUP_ENTITY_DOCUMENTATION_HPP_INCLUDED

#include <cppast/cpp_entity.hpp>

#include "documentation_entity.hpp"

namespace standardese::output::markup
{
    /// A container containing the documentation of a single entity.
    ///
    /// It can optionally have a [standardese::markup::heading]() which will be rendered as well.
    /// If it has a heading, it will also render a [standardese::markup::thematic_break]() at the
    /// end. \notes This does not represent the documentation of a file, use
    /// [standardese::markup::file_documentation]() for that.
    class entity_documentation final : public documentation_entity,
                                       public container_entity<entity_documentation>
    {
    public:
        /// Builds the documentation of an entity.
        class builder : public documentation_builder<container_builder<entity_documentation>>
        {
        public:
            /// \effects Creates it giving the id, header and synopsis.
            /// \requires The user data of the entity must either be `nullptr` or the corresponding
            /// [standardese::doc_entity]().
            builder(type_safe::object_ref<const cppast::cpp_entity> entity, block_id id,
                    type_safe::optional<documentation_header> h,
                    std::unique_ptr<code_block>               synopsis)
            : documentation_builder(std::unique_ptr<entity_documentation>(
                  new entity_documentation(entity, std::move(id), std::move(h),
                                           std::move(synopsis))))
            {}
        };

        const cppast::cpp_entity& entity() const noexcept
        {
            return *entity_;
        }

    private:
        entity_documentation(type_safe::object_ref<const cppast::cpp_entity> entity, block_id id,
                             type_safe::optional<documentation_header> h,
                             std::unique_ptr<code_block>               synopsis)
        : documentation_entity(std::move(id), std::move(h), std::move(synopsis)), entity_(entity)
        {}

        entity_kind do_get_kind() const noexcept override;

        void do_visit(visitor_callback_t cb, void* mem) const override;

        std::unique_ptr<markup::entity> do_clone() const override;

        type_safe::object_ref<const cppast::cpp_entity> entity_;
    };
}

#endif

