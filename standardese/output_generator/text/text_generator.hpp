// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_OUTPUT_TEXT_TEXT_GENERATOR_HPP_INCLUDED
#define STANDARDESE_OUTPUT_TEXT_TEXT_GENERATOR_HPP_INCLUDED

#include <cmark-gfm.h>

#include "../cmark_generator.hpp"

namespace standardese::output_generator::text
{

class text_generator : public cmark_generator {
  public:
    struct text_generator_options {
      text_generator_options();
    };

    text_generator(std::ostream& os, text_generator_options = {});
    
    ~text_generator() override;

    static std::string render(const model::entity& root);

  private:
    text_generator_options options;
};

}

#endif
