// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_MODEL_MODULE_DOCUMENTATION_HPP_INCLUDED
#define STANDARDESE_MODEL_MODULE_DOCUMENTATION_HPP_INCLUDED

#include "mixin/visitable.hpp"
#include "mixin/documentation.hpp"

namespace standardese::model
{
    /// The documentation of a module.
    class module final : public mixin::visitable<module>, public mixin::documentation
    {
      public:
        template <typename ...Args>
        module(std::string name, Args&&... args) : name(std::move(name)), mixin::documentation(std::forward<Args>(args)...) {}

        std::string name;
    };
}

#endif

