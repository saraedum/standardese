// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_MODEL_MIXIN_INDEX_HPP_INCLUDED
#define STANDARDESE_MODEL_MIXIN_INDEX_HPP_INCLUDED

#include "anchored_container.hpp"
#include "../heading.hpp"

namespace standardese::model::mixin
{
    template <typename T>
    class index : public anchored_container<T> {
    public:
        template <typename ...Args>
        index(std::string id, heading heading, Args&&... children) : anchored_container<T>(std::move(id), std::forward<Args>(children)...), heading_(std::move(heading)) {}

    private:
        heading heading_;
    };


}

#endif
