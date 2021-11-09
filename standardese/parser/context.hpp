// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//               2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_PARSER_CPP_CONTEXT_HPP_INCLUDED
#define STANDARDESE_PARSER_CPP_CONTEXT_HPP_INCLUDED

#include <memory>
#include <vector>

#include <cppast/cpp_entity_index.hpp>

namespace standardese::parser {

/// Keeps track of global state of the C++ parser.
/// The AST [cppast::cpp_entity]() objects created by the `cppast` library have
/// their lifetime bound to the containing [cppast::cpp_file]().
/// Furthermore, the cppast parser uses a global [cppast::cpp_entity_index]()
/// to track things such as type names globally. Since it is tedious to always
/// pass around an entity and its corresponding index and make sure that the
/// [cppast:cpp_file]() stays alive, we encapsulate this global data in this
/// object which is copyable and can be easily passed around where necessary.
/// \notes We could have wrapped [cppast::cpp_entity]() instead with a shared
/// pointer to the [cppast::cpp_file]() and a shared pointer to the
/// [cppast::cpp_entity_index]() instead. However, since [cppast::cpp_entity]()
/// lives in a large inheritance tree, that approach would have created a lot
/// of syntactic overhead since we have to cast the underlying types all the
/// time.
class cpp_context {
 public:
  const cppast::cpp_entity_index& index() const;

 private:
  cpp_context();

  struct context {
    std::vector<std::unique_ptr<cppast::cpp_file>> headers;
    cppast::cpp_entity_index index;
  };

  std::shared_ptr<context> context;

  friend class cppast_parser;
};

}

#endif

