// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <cppast/code_generator.hpp>
#include <cppast/cpp_class.hpp>

#include "../../standardese/transformation/exclude_uncommented_transformation.hpp"
#include "../../standardese/model/visitor/recursive_visitor.hpp"
#include "../../standardese/model/entity.hpp"
#include "../../standardese/model/cpp_entity_documentation.hpp"

namespace standardese::transformation {
namespace {

// TODO: Move implementation to the bottom and only define the class here.
struct visitor : public model::visitor::recursive_visitor<false> {
  void visit(model::cpp_entity_documentation& documentation) override {
    // TODO: Make configurable

    if (documentation.exclude_mode == model::exclude_mode::uncommented)
      // TODO: Actually delete this entity from the parent instead (how?) -- use a generic visitor, see output_group/section_transformation.
      documentation.clear();

    model::visitor::recursive_visitor<false>::visit(documentation);
  }
};

}

void exclude_uncommented_transformation::do_transform(model::entity& entity) {
  visitor v{};
  entity.accept(v);
}

}
