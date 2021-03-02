// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//               2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_PARSER_CPP_CONTEXT_HPP_INCLUDED
#define STANDARDESE_PARSER_CPP_CONTEXT_HPP_INCLUDED

#include <memory>

#include <cppast/forward.hpp>

namespace standardese::parser {

/// Global state of the C++ parser.
/// The AST [cppast::cpp_entity]() objects created by the `cppast` library have
/// their lifetime bound to the containing [cppast::cpp_file]().
/// Furthermore, the cppast parser uses a global [cppast::cpp_entity_index]()
/// to track things such as type names globally. Since it is tedious to always
/// pass around an entity and its corresponding index and make sure that the
/// [cppast:cpp_file]() stays alive, we encapsulate this global data in this
/// object which is copyable and can be easily passed around where necessary.
class cpp_context {
 public:
  const cppast::cpp_entity_index& index() const;

  friend class cppast_parser;

 private:
  struct context;

  cpp_context();

  /// Register a file's AST with this context and return the entity
  /// corresponding to this file.
  /// \notes This operation is thread safe.
  const cppast::cpp_file& add(std::unique_ptr<const cppast::cpp_file>);

  std::shared_ptr<context> context;
};

}

#endif

