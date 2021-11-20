// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include "../../standardese/transformer/inner_transformer.hpp"
#include "../../standardese/model/entity_set.hpp"
#include "../../standardese/threading/for_each.hpp"

namespace standardese::transformer {

inner_transformer::inner_transformer(model::entity_set* entities) : entities(entities) {}

void inner_transformer::transform(threading::pool::factory workers) {
  threading::for_each(workers, entities->begin(), entities->end(), [this](auto& e) {
    // An inner transformer modifies the entities stored in an entity set.
    // Since hashing and equality of such a set are very strict the
    // transformation won't break either and it is therefore safe to cast away
    // constness here, see comments in
    // [model::cpp_entity_documentation]() and [model::module]().
    do_transform(const_cast<model::entity&>(e));
  });
}

}

