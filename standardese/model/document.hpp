// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_MODEL_DOCUMENT_HPP_INCLUDED
#define STANDARDESE_MODEL_DOCUMENT_HPP_INCLUDED

#include "mixin/visitable.hpp"
#include "mixin/anchored_container.hpp"

namespace standardese::model
{
    /// An output file, e.g., the description of a header file.
    class document final : public mixin::visitable<document>, public mixin::anchored_container<> {
      public:
        template <typename ...Args>
        document(std::string name, std::string path, Args&&... args) : name(std::move(name)), path(std::move(path)), mixin::anchored_container<>(std::forward<Args>(args)...) {}

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

