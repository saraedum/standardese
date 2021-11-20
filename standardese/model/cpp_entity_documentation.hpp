// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_MODEL_CPP_ENTITY_DOCUMENTATION_HPP_INCLUDED
#define STANDARDESE_MODEL_CPP_ENTITY_DOCUMENTATION_HPP_INCLUDED

#include <cppast/cpp_entity.hpp>
#include <cppast/cpp_entity_kind.hpp>

#include <type_safe/reference.hpp>

#include "mixin/documentation.hpp"

#include "../parser/commands/section_command.hpp"
#include "../parser/cpp_context.hpp"

namespace standardese::model
{
    /// Documentation of a single C++ entity.
    class cpp_entity_documentation final : public mixin::documentation,
                                       public mixin::visitable<cpp_entity_documentation>
    {
    public:
        /// Create a documentation entity that provides the documentation of
        /// the C++ [entity]().
        explicit cpp_entity_documentation(const cppast::cpp_entity* entity, const parser::cpp_context& context, std::initializer_list<model::entity> children={});

        const cppast::cpp_entity& entity() const;

        const parser::cpp_context& context() const;

        /// The base name of the generated documentation file in the output,
        /// e.g., `header` for `header.hpp`.
        std::string output_name;

    private:
        // This field is const so there is no way to break
        // [cppast_entity_hash]() and [cppast_entity_equality]() for
        // elements stored in an [entity_set]().
        const cppast::cpp_entity* const entity_;

        parser::cpp_context context_;
    };
}

#endif
