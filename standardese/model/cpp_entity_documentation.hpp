// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_MODEL_CPPAST_DOCUMENTATION_HPP_INCLUDED
#define STANDARDESE_MODEL_CPPAST_DOCUMENTATION_HPP_INCLUDED

#include <cppast/cpp_entity.hpp>
#include <cppast/cpp_entity_kind.hpp>

#include <type_safe/reference.hpp>
#include <type_safe/optional.hpp>

#include "mixin/documentation.hpp"
#include "section.hpp"

#include "../parser/commands/section_command.hpp"
#include "../parser/cpp_context.hpp"

namespace standardese::model
{
    /// Documentation of a single C++ entity.
    class cpp_entity_documentation final : public mixin::documentation,
                                       public mixin::visitable<cpp_entity_documentation>
    {
    public:
        template <typename ...Args>
        explicit cpp_entity_documentation(const cppast::cpp_entity& entity, parser::cpp_context context, Args&&... children)
        : entity_(entity), context_(std::move(context)), mixin::documentation(std::forward<Args>(children)...) {}

        const cppast::cpp_entity& entity() const { return *entity_; }

        const parser::cpp_context& context() const { return context_; }

        type_safe::optional_ref<model::section> section(parser::commands::section_command type) {
            for (auto& child : *this)
                if (child.is<model::section>() && child.as<model::section>().type == type)
                    return type_safe::ref(child.as<model::section>());
            return type_safe::nullopt;
        }

        /// The base name of the generated documentation file in the output,
        /// e.g., `header` for `header.hpp`.
        std::string output_name;

    private:
        type_safe::object_ref<const cppast::cpp_entity> entity_;
        parser::cpp_context context_;
    };
}

#endif
