// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_TRANSFORMER_EXCLUDE_PATTERN_TRANSFORMER_HPP_INCLUDED
#define STANDARDESE_TRANSFORMER_EXCLUDE_PATTERN_TRANSFORMER_HPP_INCLUDED

#include <regex>

#include "inner_transformer.hpp"

namespace standardese::transformer {

// TODO(0.6.0-alpha): Implement me.

/// Exclude entities from the output if they match a regular expression.
class exclude_pattern_transformer : public inner_transformer {
  public:
    struct exclude_pattern_transformer_options {
      std::vector<std::regex> excluded;
    };

  protected:
    void do_transform(model::entity& root) override;
};

}

#endif

