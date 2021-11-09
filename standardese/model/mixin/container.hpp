// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_MODEL_MIXIN_CONTAINER_HPP_INCLUDED
#define STANDARDESE_MODEL_MIXIN_CONTAINER_HPP_INCLUDED

#include <vector>

#include "../entity.hpp"
#include "../markup/text.hpp"

namespace standardese::model::mixin
{
    /// A base class for entities that are containers.
    /// \tparam T the kinds of entities stored in this container.
    /// Currently, this is almost always just [entity]().
    template <typename T = entity>
    class container
    {
    public:
        using entity = T;

        container() noexcept = default;

        explicit container(std::initializer_list<T> children) : children(std::move(children)) {}

        std::vector<T> children;
    };
}

#endif
