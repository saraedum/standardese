// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include "../../standardese/model/cpp_entity_documentation.hpp"

namespace standardese::model {

cpp_entity_documentation::cpp_entity_documentation(const cppast::cpp_entity* entity, const parser::cpp_context& context, std::initializer_list<model::entity> children) : mixin::documentation(std::move(children)), entity_(entity), context_(context) {
}

const cppast::cpp_entity& cpp_entity_documentation::entity() const {
  return *entity_;
}

const parser::cpp_context& cpp_entity_documentation::context() const {
  return context_;
}

}
