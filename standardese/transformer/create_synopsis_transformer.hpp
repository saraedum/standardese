// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_TRANSFORMER_CREATE_SYNOPSIS_TRANSFORMER_HPP_INCLUDED
#define STANDARDESE_TRANSFORMER_CREATE_SYNOPSIS_TRANSFORMER_HPP_INCLUDED

#include <regex>

#include "inner_transformer.hpp"

namespace standardese::transformer {

/// Create a synopsis section for each entity.
class create_synopsis_transformer : public inner_transformer {
  public:
    struct create_synopsis_transformer_options {
      // TODO(0.6.0-alpha): Actually use this.
      bool exclude_uncommented = false;
    };

    using inner_transformer::inner_transformer;

  protected:
    void do_transform(model::entity& root) override;
};

}

#endif
