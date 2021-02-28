// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_OUTPUT_MARKUP_SUBDOCUMENT_HPP_INCLUDED
#define STANDARDESE_OUTPUT_MARKUP_SUBDOCUMENT_HPP_INCLUDED

#include "document_entity.hpp"

namespace standardese::output::markup
{
    /// A document which is not the main output page of the documentation.
    class subdocument final : public document_entity
    {
    public:
        /// Builds the sub document.
        class builder : public container_builder<subdocument>
        {
        public:
            /// \effects Creates an empty document with given title and output file name.
            builder(std::string title, std::string output_name)
            : container_builder(std::unique_ptr<subdocument>(
                  new subdocument(std::move(title), std::move(output_name))))
            {}

            using container_builder::peek;
        };

    private:
        entity_kind do_get_kind() const noexcept override;

        std::unique_ptr<entity> do_clone() const override;

        subdocument(std::string title, std::string name)
        : document_entity(std::move(title), markup::output_name::from_name(std::move(name)))
        {}
    };
}

#endif

