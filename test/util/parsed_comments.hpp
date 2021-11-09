// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_TEST_UTIL_PARSED_COMMENTS_HPP_INCLUDED
#define STANDARDESE_TEST_UTIL_PARSED_COMMENTS_HPP_INCLUDED

#include "cpp_file.hpp"

#include "../../standardese/parser/comment_parser.hpp"
#include "../../standardese/model/unordered_entities.hpp"

namespace standardese::test::util {

/// Parsed comments in a C++ header file.
class parsed_comments {
  public:
    explicit parsed_comments(const cpp_file&);

    /// Register `comment` as documenting `target`.
    parsed_comments&& add(const cppast::cpp_entity& target, const std::string& comment, parser::comment_parser_options = {}) &&;

    /// Return the result of parsing for a C++ entity (assuming that there is a unique one.)
    model::cpp_entity_documentation as_documentation() const;

    /// Return the result of parsing a module documentation (assuming that there is a unique one.)
    model::module as_module() const;

    /// Return the parsed entity (assuming that there is a unique one.)
    operator model::entity() const;

    /// Return the documentation for `target`.
    model::entity operator[](type_safe::object_ref<const cppast::cpp_entity> target) const;

    /// Return the documentation for the entity `name`.
    model::entity operator[](const std::string& name) const;

    /// The entities parsed so far; includes placeholder entries for uncommented entities.
    model::unordered_entities entities;

  private:
    const cpp_file& header;
};

}

#endif

