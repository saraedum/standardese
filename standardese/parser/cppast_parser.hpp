// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//               2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_PARSER_CPPAST_PARSER_HPP_INCLUDED
#define STANDARDESE_PARSER_CPPAST_PARSER_HPP_INCLUDED

#include <cppast/cpp_entity_index.hpp>
#include <cppast/forward.hpp>
#include <cppast/libclang_parser.hpp>
#include <boost/filesystem.hpp>

#include "../threading/unthreaded_pool.hpp"
#include "cpp_context.hpp"

namespace standardese::parser {

/// A C++ Parser.
/// Calls the clang parser through
/// [cppast](https://github.com/foonathan/cppast) and provides an AST of C/C++
/// source code.
class cppast_parser {
 public:
  struct options {
    options();

    /// Flags for the C++ Parser.
    cppast::libclang_compile_config clang_config;

    /// Directory containing a `compile_commands.json` compilation database.
    type_safe::optional<boost::filesystem::path> compile_commands;
  };

  explicit cppast_parser(options);

  /// Parse a C++ file and return the root of its AST.
  /// \throws An exception if a parse error happens.
  /// \notes This operation is thread-safe.
  const cppast::cpp_file& parse(const boost::filesystem::path&);

  const cpp_context& context() const;

 private:
  options options;

  cpp_context context_;

  type_safe::optional<cppast::libclang_compilation_database> compile_commands;
  cppast::libclang_parser parser; 
};

}

#endif
