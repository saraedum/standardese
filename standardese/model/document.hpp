// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_MODEL_DOCUMENT_HPP_INCLUDED
#define STANDARDESE_MODEL_DOCUMENT_HPP_INCLUDED

#include "mixin/visitable.hpp"
#include "mixin/anchored.hpp"
#include "mixin/container.hpp"

namespace standardese::model
{
    /// An output file, e.g., the description of a header file.
    class document final : public mixin::visitable<document>, public mixin::anchored, public mixin::container<> {
      public:
        document(std::string name, std::string path, std::initializer_list<model::entity> children={}) : name(std::move(name)), path(std::move(path)), mixin::container<>(std::move(children)) {}

        /// Return a container for block entities.
        /// Internally, it is often necessary to store some paragraphs or other
        /// blocks in a container. We use such an unnamed document for this
        /// purpose.
        static document anonymous(std::initializer_list<model::entity> children={}) {
          return document{"", "", std::move(children)};
        }

        /// Return the unique block of this document.
        /// Returns an empty paragraph if this document is empty.
        model::entity block() const;

        /// Return the unique block of this document which must be a paragraph.
        /// Return an empty paragraph if this document is empty.
        /// This is essentially equivalent to `block().as<paragraph>()`.
        model::markup::paragraph paragraph() const;

        /// A (unique) symbolic base name for this document.
        /// The final output name without the suffix.
        std::string name;

        /// The relative path of this document relative to some document
        /// root for the purpose of linking such as the relative URL of some
        /// HTTP endpoint that will serve this document eventually.
        std::string path;
    };
}

#endif

