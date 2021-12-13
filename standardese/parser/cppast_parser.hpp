// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//               2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_PARSER_CPPAST_PARSER_HPP_INCLUDED
#define STANDARDESE_PARSER_CPPAST_PARSER_HPP_INCLUDED

#include <cppast/cpp_entity_index.hpp>
#include <cppast/cppast_fwd.hpp>
#include <cppast/libclang_parser.hpp>
#include <boost/filesystem.hpp>
#include <optional>

#include "../threading/unthreaded_pool.hpp"
#include "cpp_context.hpp"

namespace standardese::parser {

/// A C++ Parser.
/// Calls the clang parser through
/// [cppast](https://github.com/foonathan/cppast) and provides an AST of C/C++
/// source code.
class cppast_parser {
 public:
  struct cppast_parser_options {
    cppast_parser_options();

    /// Flags for the C++ Parser.
    cppast::libclang_compile_config clang_config;

    /// Path of a `compile_commands.json` compilation database, see
    /// https://clang.llvm.org/docs/JSONCompilationDatabase.html#json-compilation-database-format-specification.
    std::optional<boost::filesystem::path> compile_commands;

    /// Path of a `compile_flags.txt` compilation database, see
    /// https://clang.llvm.org/docs/JSONCompilationDatabase.html#alternatives
    /// This argument is ignored since the format is not supported by cppast yet.
    std::optional<boost::filesystem::path> compile_flags;
  };

  explicit cppast_parser(cppast_parser_options={});

  /// Parse a C++ file and return the root of its AST.
  /// \throws An exception if a parse error happens.
  /// \notes This operation is thread-safe.
  const cppast::cpp_file& parse(const boost::filesystem::path&);

  const cpp_context& context() const;

 private:
  cppast_parser_options options;

  cpp_context context_;

  std::optional<cppast::libclang_compilation_database> compile_commands;
  cppast::libclang_parser parser;
};

}

#endif
