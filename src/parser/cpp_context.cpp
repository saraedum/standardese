// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//               2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <memory>
#include <vector>
#include <mutex>

#include <cppast/cpp_entity_index.hpp>
#include <cppast/cpp_file.hpp>

#include "../../standardese/parser/cpp_context.hpp"

namespace standardese::parser {

struct cpp_context::context {
  std::vector<std::unique_ptr<const cppast::cpp_file>> headers;
  cppast::cpp_entity_index index;
  std::mutex mutex;
};

const cppast::cpp_entity_index& cpp_context::index() const {
  return context->index;
}

cpp_context::cpp_context() : context(new struct cpp_context::context()) {}

const cppast::cpp_file& cpp_context::add(std::unique_ptr<const cppast::cpp_file> header) {
  std::lock_guard lock{context->mutex};

  context->headers.push_back(std::move(header));
  return **context->headers.rbegin();
}

}
