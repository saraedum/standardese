// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_OUTPUT_MARKUP_FILE_INDEX_HPP_INCLUDED
#define STANDARDESE_OUTPUT_MARKUP_FILE_INDEX_HPP_INCLUDED

#include "index_entity.hpp"
#include "entity_index_item.hpp"

namespace standardese::output::markup
{
    /// The index of all files.
    /// \notes This is created by the [standardese::file_index]().
    class file_index final : public index_entity, public container_entity<entity_index_item>
    {
    public:
        /// Builds the file index.
        class builder : public container_builder<file_index>
        {
        public:
            /// \effects Creates it given the heading.
            builder(std::unique_ptr<markup::heading> h)
            : container_builder(std::unique_ptr<file_index>(new file_index(std::move(h))))
            {}
        };

    private:
        file_index(std::unique_ptr<markup::heading> h)
        : index_entity(block_id("file-index"), std::move(h))
        {}

        entity_kind do_get_kind() const noexcept override;

        void do_visit(visitor_callback_t cb, void* mem) const override;

        std::unique_ptr<entity> do_clone() const override;
    };
}

#endif

