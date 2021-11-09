// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_MODEL_MIXIN_DOCUMENT_HPP_INCLUDED
#define STANDARDESE_MODEL_MIXIN_DOCUMENT_HPP_INCLUDED

#include <string>

#include "anchored.hpp"
#include "container.hpp"

namespace standardese::model::mixin {

    // A top-level document that will eventually be rendered as a single output file.
    class document : public anchored, public container<model::entity> {
    public:
        explicit document(std::string path, std::initializer_list<model::entity> children) : path(std::move(path)), container<model::entity>(std::move(children)) {}

        std::string path;
    };
}

#endif
