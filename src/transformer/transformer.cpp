// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include "../../standardese/transformer/transformer.hpp"
#include "../../standardese/model/unordered_entities.hpp"
#include "../../standardese/threading/for_each.hpp"

namespace standardese::transformer {

transformer::transformer(model::unordered_entities& entities) : entities(entities) {}

void transformer::transform(threading::pool::factory workers) {
  threading::for_each(workers, entities.begin(), entities.end(), [this](auto& e) {
    do_transform(e);
  });
}

}

