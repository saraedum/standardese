// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_PARSER_PARSE_ERROR_HPP_INCLUDED
#define STANDARDESE_PARSER_PARSE_ERROR_HPP_INCLUDED

#include <stdexcept>

#include <type_safe/reference.hpp>
#include <fmt/core.h>
#include <cppast/cpp_entity.hpp>

#include "../model/module.hpp"
#include "../model/cpp_entity_documentation.hpp"

extern "C" {
    typedef struct cmark_parser cmark_parser;
    typedef struct cmark_node cmark_node;
}

namespace standardese::parser
{
    /// An error while parsing a C++ comment.
    class parse_error : public std::runtime_error
    {
        template <typename T>
        static auto formattable(type_safe::object_ref<T> value) {
            return formattable(*value);
        }

        template <typename T>
        static auto&& formattable(T&& value) {
            return std::forward<T>(value);
        }

        static std::string formattable(const cppast::cpp_entity& value) {
            return value.name();
        }

        // TODO: Maybe implement proper fmt formatting instead.
        static std::string formattable(model::cpp_entity_documentation& value) {
            return formattable(value.entity());
        }

        static std::string formattable(const model::cpp_entity_documentation& value) {
            return formattable(value.entity());
        }

        static std::string formattable(model::module& value) {
            return formattable(value.name);
        }

        static std::string formattable(const model::module& value) {
            return formattable(value.name);
        }

    public:
        template <typename ...Args>
        explicit parse_error(const std::string& format_str, Args&&... args) : std::runtime_error(fmt::format(format_str, formattable(std::forward<Args>(args))...)) {}

        // TODO: Properly report where this node is in the source file. (Needs changes in cppast probably.)
        template <typename ...Args>
        explicit parse_error(cmark_node* location, const std::string& format_str, Args&&... args) : parse_error(format_str, std::forward<Args>(args)...) {}
    };
}

#endif
