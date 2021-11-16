// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_TRANSFORMER_CREATE_OUTPUT_SECTION_HEADING_TRANSFORMER_HPP_INCLUDED
#define STANDARDESE_TRANSFORMER_CREATE_OUTPUT_SECTION_HEADING_TRANSFORMER_HPP_INCLUDED

#include "inner_transformer.hpp"

namespace standardese::transformer {

/// Create headings for entities that are grouped in an output section.
class create_output_section_heading_transformer : public inner_transformer {
  public:
    using inner_transformer::inner_transformer;

  protected:
    void do_transform(model::entity& root) override;
};

}

#endif
