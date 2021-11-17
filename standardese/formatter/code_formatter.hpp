// Copyright (C) 2017 Jonathan Müller <jonathanmueller.dev@gmail.com>
//               2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_FORMATTER_CODE_FORMATTER_HPP_INCLUDED
#define STANDARDESE_FORMATTER_CODE_FORMATTER_HPP_INCLUDED

#include <string>
#include <cppast/cppast_fwd.hpp>

#include "inja_formatter.hpp"
#include "../parser/cpp_context.hpp"

namespace standardese::formatter {

/// Turns a C++ AST into something ressembling source code.
class code_formatter {
 public:
  struct code_formatter_options : inja_formatter::inja_formatter_options {
    code_formatter_options();
  };

  code_formatter(code_formatter_options, const parser::cpp_context&);

  // TODO(0.6.0-beta): Make the context more flexible. It could be any kind of documentation.

  /// Return `entity` written in C++ syntax.
  model::markup::paragraph build(const cppast::cpp_entity& entity) const;

  /// Return `entity` written in C++ syntax.
  model::markup::paragraph build(const cppast::cpp_entity& entity, const cppast::cpp_entity& context) const;

  /// Return `entity` written in C++ syntax according to `format`.
  model::markup::paragraph build(const std::string& format, const cppast::cpp_entity& entity) const;

  /// Return `entity` written in C++ syntax according to `format`.
  model::markup::paragraph build(const std::string& format, const cppast::cpp_entity& entity, const cppast::cpp_entity& context) const;

  /// Return `type` written in C++ syntax.
  model::markup::paragraph build(const cppast::cpp_type& type) const;

  /// Return `type` written in C++ syntax.
  model::markup::paragraph build(const cppast::cpp_type& type, const cppast::cpp_entity& context) const;

  /// Return `type` written in C++ syntax according to `format`.
  model::markup::paragraph build(const std::string& format, const cppast::cpp_type& type) const;

  /// Return `type` written in C++ syntax according to `format`.
  model::markup::paragraph build(const std::string& format, const cppast::cpp_type& type, const cppast::cpp_entity& context) const;

 private:
  code_formatter_options options;
  parser::cpp_context cpp_context;
};

}

#endif
