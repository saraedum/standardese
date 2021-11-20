// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include "../../standardese/transformer/outer_transformer.hpp"
#include "../../standardese/model/entity_set.hpp"
#include "../../standardese/model/entity.hpp"
#include "../../standardese/threading/transform.hpp"

namespace standardese::transformer {

outer_transformer::outer_transformer(const model::entity_set* entities) : entities(entities) {}

model::entity_set outer_transformer::transform(threading::pool::factory workers) {
  auto transformed = threading::transform(workers, entities->begin(), entities->end(), [this](auto& e) {
    return do_transform(e);
  });

  return merge(std::move(transformed));
}

model::entity_set outer_transformer::merge(std::vector<std::vector<model::entity>>&& transformed) const {
  model::entity_set merged;

  entity_set_extend(merged, std::move(transformed));

  return merged;
}

}

