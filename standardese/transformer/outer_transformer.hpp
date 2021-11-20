// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_TRANSFORMER_OUTER_TRANSFORMER_HPP_INCLUDED
#define STANDARDESE_TRANSFORMER_OUTER_TRANSFORMER_HPP_INCLUDED

#include <vector>

#include "../threading/unthreaded_pool.hpp"

namespace standardese::transformer {

/// Base class for transformers that produces some (possibly zero) entities for
/// each original entity but does not modify the existing entities (unlike the
/// [inner_transformer]().)
class outer_transformer {
  public:
    /// Create a transformer that creates new entities.
    /// The database of existing [entities]() must be kept alive until [transform]() has been called.
    explicit outer_transformer(const model::entity_set* entities);

    /// Transform the entities passed in the constructor.
    model::entity_set transform(threading::pool::factory workers=threading::unthreaded_pool::factory);

  protected:
    /// Return the entities to replace `root` with.
    /// This function might be called in parallel so implementations must not
    /// use any global state that is not thread-safe.
    virtual std::vector<model::entity> do_transform(const model::entity& root) const = 0;

    virtual model::entity_set merge(std::vector<std::vector<model::entity>>&&) const;

    /// The set of entities we are working on.
    const model::entity_set* entities;
};

}

#endif
