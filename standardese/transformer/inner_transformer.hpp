// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_TRANSFORMER_INNER_TRANSFORMER_HPP_INCLUDED
#define STANDARDESE_TRANSFORMER_INNER_TRANSFORMER_HPP_INCLUDED

#include "../threading/unthreaded_pool.hpp"

namespace standardese::transformer {

/// Base class for transformers that modify entities but do not change the set
/// of entities.
class inner_transformer {
  public:
    /// Create a transformer that modifies [entities]().
    /// Entities will actually be modified in [transform]() so the set of
    /// entities must be kept alive until [transform]() was called.
    explicit inner_transformer(model::unordered_entities* entities);

    /// Transform the entities passed in the constructor.
    void transform(threading::pool::factory workers=threading::unthreaded_pool::factory);

  protected:
    /// Perform changes on the entity `root`.
    /// This function might be called in parallel so implementations must not
    /// use any global state that is not thread safe.
    virtual void do_transform(model::entity& root) = 0;

  private:
    /// The set of entities we are working on.
    model::unordered_entities* entities;
};

}

#endif
