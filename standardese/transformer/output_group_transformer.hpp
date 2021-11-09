// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_TRANSFORMER_OUTPUT_GROUP_TRANSFORMER_HPP_INCLUDED
#define STANDARDESE_TRANSFORMER_OUTPUT_GROUP_TRANSFORMER_HPP_INCLUDED

#include "transformer.hpp"

namespace standardese::transformer {

// TODO(0.6.0-alpha): Rename to output_section_transformer
class output_group_transformer : public transformer {
  public:
    using transformer::transformer;

  protected:
    void do_transform(model::entity& root) override;
};

}

#endif
