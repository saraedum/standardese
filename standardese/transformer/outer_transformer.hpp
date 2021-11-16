// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_TRANSFORMER_OUTER_TRANSFORMER_HPP_INCLUDED
#define STANDARDESE_TRANSFORMER_OUTER_TRANSFORMER_HPP_INCLUDED

#include <vector>

#include "../threading/unthreaded_pool.hpp"

namespace standardese::transformer {

/// Base class for transformers that completely rewrite entities by replacing
/// each entity by some (potentially zero) replacement entities.
class outer_transformer {
  public:
    explicit outer_transformer(model::unordered_entities& entities);

    /// Transform the entities passed in the constructor.
    void transform(threading::pool::factory workers=threading::unthreaded_pool::factory);

  protected:
    /// Return the entities to replace `root` with.
    /// This function might be called in parallel so implementations must not
    /// use any global state that is not thread-safe.
    virtual std::vector<model::entity> do_transform(model::entity& root) = 0;

  private:
    /// The set of entities we are working on.
    model::unordered_entities& entities;
};

}

#endif
