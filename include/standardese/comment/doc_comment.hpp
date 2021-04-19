// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_COMMENT_DOC_COMMENT_HPP_INCLUDED
#define STANDARDESE_COMMENT_DOC_COMMENT_HPP_INCLUDED

#include "metadata.hpp"

#include "../output/markup/doc_section.hpp"
#include "../output/markup/brief_section.hpp"
#include "../output/markup/entity_documentation.hpp"
#include "../output/markup/file_documentation.hpp"
#include "../output/markup/namespace_documentation.hpp"
#include "../output/markup/module_documentation.hpp"

namespace standardese
{
namespace comment
{
    /// The comment associated with an entity.
    class doc_comment
    {
        using section_iterator = output::markup::container_entity<output::markup::doc_section>::iterator;

        class section_range
        {
        public:
            section_range(const std::vector<std::unique_ptr<output::markup::doc_section>>& sections)
            : begin_(sections.begin()), end_(sections.end())
            {}

            bool empty() const noexcept
            {
                return begin_ == end_;
            }

            std::size_t size() const noexcept
            {
                return std::size_t(end_ - begin_);
            }

            section_iterator begin() const noexcept
            {
                return begin_;
            }

            section_iterator end() const noexcept
            {
                return end_;
            }

        private:
            std::vector<std::unique_ptr<output::markup::doc_section>>::const_iterator begin_, end_;
        };

    public:
        /// \effects Creates it giving the metadata and all the sections.
        /// \requires Sections must not contain the brief section.
        doc_comment(comment::metadata metadata, std::unique_ptr<output::markup::brief_section> brief,
                    std::vector<std::unique_ptr<output::markup::doc_section>> sections)
        : metadata_(std::move(metadata)), sections_(std::move(sections)), brief_(std::move(brief))
        {}

        /// \returns The metadata of the comment.
        /// \group metadata
        const comment::metadata& metadata() const noexcept
        {
            return metadata_;
        }

        /// \group metadata
        comment::metadata& metadata() noexcept
        {
            return metadata_;
        }

        /// \returns The non-brief documentation sections.
        section_range sections() const noexcept
        {
            return section_range(sections_);
        }

        /// \returns A reference to the brief section, if there is one.
        type_safe::optional_ref<const output::markup::brief_section> brief_section() const noexcept
        {
            return type_safe::opt_ref(brief_.get());
        }

    private:
        comment::metadata                                 metadata_;
        std::vector<std::unique_ptr<output::markup::doc_section>> sections_;
        std::unique_ptr<output::markup::brief_section>            brief_;

        friend doc_comment merge(comment::metadata data, doc_comment&& other);
    };

    /// Merges data and a comment.
    /// \returns A documentation comment containing the merged metadata and documentation from the
    /// other comment. The merged metadata is all the things set in `data` and things set in
    /// `other.metadata()`, which aren't set in `data`.
    doc_comment merge(metadata data, doc_comment&& other);

    /// \effects Adds a copy of the sections to the documentation builder.
    /// \group set_sections
    void set_sections(output::markup::entity_documentation::builder& builder, const doc_comment& comment);

    /// \group set_sections
    void set_sections(output::markup::file_documentation::builder& builder, const doc_comment& comment);

    /// \group set_sections
    void set_sections(output::markup::namespace_documentation::builder& builder,
                      const doc_comment&                        comment);

    /// \group set_sections
    void set_sections(output::markup::module_documentation::builder& builder, const doc_comment& comment);
} // namespace comment
} // namespace standardese

#endif // STANDARDESE_COMMENT_DOC_COMMENT_HPP_INCLUDED
