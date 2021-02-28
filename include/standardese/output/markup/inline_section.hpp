// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//               2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_OUTPUT_MARKUP_INLINE_SECTION_HPP_INCLUDED
#define STANDARDESE_OUTPUT_MARKUP_INLINE_SECTION_HPP_INCLUDED

#include "doc_section.hpp"
#include "paragraph.hpp"

namespace standardese::output::markup
{
    /// A section like `\effects` that just contains some text.
    ///
    /// It cannot be used for `\brief` or `\details`,
    /// use [standardese::markup::brief_section]() or [standardese::markup::details_section]() for
    /// that.
    class inline_section final : public doc_section
    {
    public:
        /// Builds a section.
        class builder
        {
        public:
            /// \effects Creates an empty section of given type and using the given name.
            /// \notes `name` should not contain the trailing `:`.
            builder(section_type type, std::string name)
            : result_(new inline_section(type, std::move(name), nullptr))
            {}

            /// \effects Adds a child to the section.
            builder& add_child(std::unique_ptr<phrasing_entity> entity)
            {
                paragraph_.add_child(std::move(entity));
                return *this;
            }

            /// \returns The finished section.
            std::unique_ptr<inline_section> finish() noexcept
            {
                result_->paragraph_ = paragraph_.finish();
                result_->set_ownership(*result_->paragraph_);
                return std::move(result_);
            }

        private:
            std::unique_ptr<inline_section> result_;
            paragraph::builder              paragraph_;
        };

        /// \returns The newly built inline section.
        /// It will use the children of paragraph for itself.
        static std::unique_ptr<inline_section> build(section_type type, std::string name,
                                                     std::unique_ptr<markup::paragraph> paragraph)
        {
            return std::unique_ptr<inline_section>(
                new inline_section(type, std::move(name), std::move(paragraph)));
        }

        /// \returns The name of the section.
        const std::string& name() const noexcept
        {
            return name_;
        }

        /// \returns An iterator to the first child.
        container_entity<phrasing_entity>::iterator begin() const noexcept
        {
            return paragraph_->begin();
        }

        /// \returns An iterator one past the last child.
        container_entity<phrasing_entity>::iterator end() const noexcept
        {
            return paragraph_->end();
        }

    private:
        entity_kind do_get_kind() const noexcept override;

        void do_visit(visitor_callback_t cb, void* mem) const override;

        std::unique_ptr<entity> do_clone() const override;

        inline_section(section_type type, std::string name,
                       std::unique_ptr<markup::paragraph> paragraph)
        : name_(std::move(name)), paragraph_(std::move(paragraph)), type_(type)
        {}

        std::string                        name_;
        std::unique_ptr<markup::paragraph> paragraph_;
        section_type                       type_;
    };
}

#endif

