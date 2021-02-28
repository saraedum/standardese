// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_OUTPUT_MARKUP_DOCUMENTATION_ENTITY_HPP_INCLUDED
#define STANDARDESE_OUTPUT_MARKUP_DOCUMENTATION_ENTITY_HPP_INCLUDED

#include "block_entity.hpp"
#include "documentation_header.hpp"
#include "code_block.hpp"
#include "brief_section.hpp"
#include "details_section.hpp"
#include "inline_section.hpp"
#include "list_section.hpp"

#include "detail/vector_ptr_range.hpp"

namespace standardese::output::markup
{
    /// The documentation of an entity.
    class documentation_entity : public block_entity
    {
    public:
        /// \returns The header of a documentation, if any.
        const type_safe::optional<documentation_header>& header() const noexcept
        {
            return header_;
        }

        /// \returns A reference to the synopsis of the entity, if there is any.
        type_safe::optional_ref<const code_block> synopsis() const noexcept
        {
            return type_safe::opt_ref(synopsis_.get());
        }

        /// \returns A reference to the `\brief` section of the documentation,
        /// if there is any.
        type_safe::optional_ref<const markup::brief_section> brief_section() const noexcept;

        /// \returns A reference to the `\details` section of the documentation,
        /// if there is any.
        type_safe::optional_ref<const markup::details_section> details_section() const noexcept;

        /// \returns An iteratable object iterating over all the sections (including brief and
        /// details), in the order they were given.
        detail::vector_ptr_range<doc_section> doc_sections() const noexcept
        {
            return {sections_.begin(), sections_.end()};
        }

    protected:
        documentation_entity(block_id id, type_safe::optional<documentation_header> h,
                             std::unique_ptr<code_block> synopsis) // may be nullptr
        : block_entity(std::move(id)), header_(std::move(h)), synopsis_(std::move(synopsis))
        {
            if (synopsis_)
                set_ownership(*synopsis_);
            if (header_)
                set_ownership(header_.value().heading());
        }

        template <class BaseBuilder>
        class documentation_builder : public BaseBuilder
        {
        public:
            /// \effects Adds a brief section.
            /// \requires This function must be called at most once.
            documentation_builder& add_brief(std::unique_ptr<markup::brief_section> section)
            {
                add_section_impl(std::move(section));
                return *this;
            }

            /// \effects Adds a details section.
            /// \requires This function must be called at most once.
            documentation_builder& add_details(std::unique_ptr<markup::details_section> section)
            {
                add_section_impl(std::move(section));
                return *this;
            }

            /// \effects Adds an inline section.
            documentation_builder& add_section(std::unique_ptr<inline_section> section)
            {
                add_section_impl(std::move(section));
                return *this;
            }

            /// \effects Adds a list section.
            documentation_builder& add_section(std::unique_ptr<list_section> section)
            {
                add_section_impl(std::move(section));
                return *this;
            }

            /// \returns The id of the documentation.
            const block_id& id() const noexcept
            {
                return this->peek().id();
            }

            /// \returns Whether or not it has any documentation sections.
            bool has_documentation() const noexcept
            {
                return !this->peek().sections_.empty();
            }

        protected:
            using BaseBuilder::BaseBuilder;

            void add_section_impl(std::unique_ptr<doc_section> section)
            {
                detail::parent_updater::set(*section, type_safe::ref(this->peek()));
                this->peek().sections_.push_back(std::move(section));
            }

            friend class entity_documentation;
            friend class file_documentation;
            friend class namespace_documentation;
            friend class module_documentation;
        };

    private:
        std::vector<std::unique_ptr<doc_section>> sections_;
        type_safe::optional<documentation_header> header_;
        std::unique_ptr<code_block>               synopsis_; // may be nullptr
    };
}

#endif

