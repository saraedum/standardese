// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_OUTPUT_GENERATOR_MARKDOWN_MARKDOWN_GENERATOR_HPP_INCLUDED
#define STANDARDESE_OUTPUT_GENERATOR_MARKDOWN_MARKDOWN_GENERATOR_HPP_INCLUDED

#include "../../markup/entity.hpp"

namespace standardese::output::generator::markdown
{

class markdown_generator {
    public:

    static std::string render(const markup::entity&);
};

}

#endif

