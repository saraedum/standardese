// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include "../../standardese/transformer/outer_transformer.hpp"
#include "../../standardese/model/unordered_entities.hpp"
#include "../../standardese/model/entity.hpp"
#include "../../standardese/threading/transform.hpp"

namespace standardese::transformer {

outer_transformer::outer_transformer(const model::unordered_entities* entities) : entities(entities) {}

model::unordered_entities outer_transformer::transform(threading::pool::factory workers) {
  model::unordered_entities transformed;

  for (auto& replacements : threading::transform(workers, entities->begin(), entities->end(), [this](auto& e) {
    return do_transform(e);
  }))
    for (auto& replacement : replacements)
      transformed.insert(std::move(replacement));

  return transformed;
}

}

