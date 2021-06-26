// Copyright (C) 2017 Jonathan Müller <jonathanmueller.dev@gmail.com>
//               2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_FORMATTER_CODE_FORMATTER_HPP_INCLUDED
#define STANDARDESE_FORMATTER_CODE_FORMATTER_HPP_INCLUDED

#include <string>
#include <cppast/forward.hpp>

#include "inja_formatter.hpp"
#include "../parser/cpp_context.hpp"

namespace standardese::formatter {

/// Turns a C++ AST into something ressembling source code.
class code_formatter {
 public:
  struct code_formatter_options : inja_formatter::inja_formatter_options {
    code_formatter_options();
  };

  code_formatter(code_formatter_options, parser::cpp_context);

  // TODO: Make the context more flexible. It could be any kind of documentation.

  /// Return `entity` written in C++ syntax.
  /// The returned document contains a single block such as a paragraph or a
  /// code block that contains the markup for the entity.
  model::document build(const cppast::cpp_entity& entity) const;

  /// Return `entity` written in C++ syntax.
  /// The returned document contains a single block such as a paragraph or a
  /// code block that contains the markup for the entity.
  model::document build(const cppast::cpp_entity& entity, const model::mixin::documentation& context) const;

  /// Return `entity` written in C++ syntax according to `format`.
  /// The returned document contains a single block such as a paragraph or a
  /// code block that contains the markup for the entity.
  model::document build(const std::string& format, const cppast::cpp_entity& entity) const;

  /// Return `entity` written in C++ syntax according to `format`.
  /// The returned document contains a single block such as a paragraph or a
  /// code block that contains the markup for the entity.
  model::document build(const std::string& format, const cppast::cpp_entity& entity, const model::mixin::documentation& context) const;

  /// Return `type` written in C++ syntax.
  /// The returned document contains a single block such as a paragraph or a
  /// code block that contains the markup for the type.
  model::document build(const cppast::cpp_type& type) const;

  /// Return `type` written in C++ syntax.
  /// The returned document contains a single block such as a paragraph or a
  /// code block that contains the markup for the type.
  model::document build(const cppast::cpp_type& type, const model::mixin::documentation& context) const;

  /// Return `type` written in C++ syntax according to `format`.
  /// The returned document contains a single block such as a paragraph or a
  /// code block that contains the markup for the type.
  model::document build(const std::string& format, const cppast::cpp_type& type) const;
  /// Return `type` written in C++ syntax according to `format`.
  /// The returned document contains a single block such as a paragraph or a
  /// code block that contains the markup for the type.
  model::document build(const std::string& format, const cppast::cpp_type& type, const model::mixin::documentation& context) const;

  /// Return `argument` written in C++ syntax.
  /// The returned document contains a single block such as a paragraph or a
  /// code block that contains the markup for the argument.
  model::document build(const cppast::cpp_template_argument& argument) const;

  /// Return `argument` written in C++ syntax.
  /// The returned document contains a single block such as a paragraph or a
  /// code block that contains the markup for the argument.
  model::document build(const cppast::cpp_template_argument& argument, const model::mixin::documentation& context) const;

  /// Return `argument` written in C++ syntax according to `format`.
  /// The returned document contains a single block such as a paragraph or a
  /// code block that contains the markup for the argument.
  model::document build(const std::string& format, const cppast::cpp_template_argument& argument) const;

  /// Return `argument` written in C++ syntax according to `format`.
  /// The returned document contains a single block such as a paragraph or a
  /// code block that contains the markup for the argument.
  model::document build(const std::string& format, const cppast::cpp_template_argument& argument, const model::mixin::documentation& context) const;

 private:
  code_formatter_options options;
  parser::cpp_context cpp_context;
};

}

#endif
