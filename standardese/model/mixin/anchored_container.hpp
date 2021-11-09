// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_MODEL_MIXIN_ANCHORED_CONTAINER_HPP_INCLUDED
#define STANDARDESE_MODEL_MIXIN_ANCHORED_CONTAINER_HPP_INCLUDED

#include "anchored.hpp"
#include "container.hpp"
#include "../entity.hpp"

namespace standardese::model::mixin
{
    // TODO(0.6.0-alpha): What is the point of this? Shouldn't we just inherit from both instead?
    template <typename T = entity>
    class anchored_container : public mixin::anchored, public mixin::container<T>
    {
    public:
        explicit anchored_container(std::initializer_list<T> children={}) : mixin::anchored(), mixin::container<T>(std::move(children)) {}
    };
}

#endif
