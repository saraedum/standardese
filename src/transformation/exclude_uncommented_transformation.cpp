// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <cppast/code_generator.hpp>
#include <cppast/cpp_class.hpp>

#include "../../standardese/transformation/exclude_uncommented_transformation.hpp"
#include "../../standardese/model/visitor/visit.hpp"
#include "../../standardese/model/entity.hpp"
#include "../../standardese/model/cpp_entity_documentation.hpp"

namespace standardese::transformation {

void exclude_uncommented_transformation::do_transform(model::entity& document) {
  model::visitor::visit([](auto&& documentation) {
    using T = std::decay_t<decltype(documentation)>;
    if constexpr (std::is_same_v<T, model::cpp_entity_documentation>) {
      // TODO: Make configurable

      if (documentation.exclude_mode == model::exclude_mode::uncommented)
        // TODO: Actually delete this entity from the parent instead (how?) -- use a generic visitor, see output_group/section_transformation.
        documentation.clear();
    }
    
    return model::visitor::recursion::RECURSE;
  }, document);
}

}
