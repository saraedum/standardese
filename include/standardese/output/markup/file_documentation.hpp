// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_OUTPUT_MARKUP_FILE_DOCUMENTATION_HPP_INCLUDED
#define STANDARDESE_OUTPUT_MARKUP_FILE_DOCUMENTATION_HPP_INCLUDED

#include <cppast/forward.hpp>

#include "documentation_entity.hpp"
#include "entity_documentation.hpp"

namespace standardese::output::markup
{
    /// The documentation of a file.
    ///
    /// \notes This does not represent a stand-alone file, use a
    /// [standardese::markup::document_entity]() for that.
    class file_documentation final : public documentation_entity,
                                     public container_entity<entity_documentation>
    {
    public:
        /// Builds the documentation of a file.
        class builder : public documentation_builder<container_builder<file_documentation>>
        {
        public:
            /// \effects Creates it giving the id, header and synopsis.
            /// \requires The user data of the file must either be `nullptr` or the corresponding
            /// [standardese::doc_entity]().
            builder(type_safe::object_ref<const cppast::cpp_file> f, block_id id,
                    type_safe::optional<documentation_header> h,
                    std::unique_ptr<code_block>               synopsis)
            : documentation_builder(std::unique_ptr<file_documentation>(
                  new file_documentation(f, std::move(id), std::move(h), std::move(synopsis))))
            {}
        };

        const cppast::cpp_file& file() const noexcept
        {
            return *file_;
        }

    private:
        file_documentation(type_safe::object_ref<const cppast::cpp_file> f, block_id id,
                           type_safe::optional<documentation_header> h,
                           std::unique_ptr<code_block>               synopsis)
        : documentation_entity(std::move(id), std::move(h), std::move(synopsis)), file_(f)
        {}

        entity_kind do_get_kind() const noexcept override;

        void do_visit(visitor_callback_t cb, void* mem) const override;

        std::unique_ptr<entity> do_clone() const override;

        type_safe::object_ref<const cppast::cpp_file> file_;
    };
}

#endif

