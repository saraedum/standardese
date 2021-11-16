// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_TRANSFORMER_GROUP_UNCOMMENTED_TRANSFORMER_HPP_INCLUDED
#define STANDARDESE_TRANSFORMER_GROUP_UNCOMMENTED_TRANSFORMER_HPP_INCLUDED

#include "inner_transformer.hpp"

namespace standardese::transformer {

/// Group uncommented entities with the preceding commented entity.
class group_uncommented_transformer : public inner_transformer {
  public:
    struct group_uncommented_options {
    };

    explicit group_uncommented_transformer(model::unordered_entities& documents, struct group_uncommented_options options);

  protected:
    void do_transform(model::entity& root) override;

  private:
    group_uncommented_options options;
};

}

#endif
