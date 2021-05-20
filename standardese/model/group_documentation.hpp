// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_MODEL_GROUP_DOCUMENTATION_HPP_INCLUDED
#define STANDARDESE_MODEL_GROUP_DOCUMENTATION_HPP_INCLUDED

#include <cppast/cpp_entity.hpp>

#include "mixin/documentation.hpp"
#include "cpp_entity_documentation.hpp"

#include "../parser/cpp_context.hpp"

namespace standardese::model {

class group_documentation final : public mixin::documentation, public mixin::visitable<group_documentation> {
  public:
    template <typename ...Args>
    explicit group_documentation(parser::cpp_context context, Args&&... children)
    : context_(std::move(context)), mixin::documentation(std::forward<Args>(children)...) {}

    const parser::cpp_context& context() const { return context_; }

    std::vector<cpp_entity_documentation> entities;

  private:
    parser::cpp_context context_;
};

}

#endif
