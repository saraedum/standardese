// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_OUTPUT_GENERATOR_HTML_HTML_GENERATOR_HPP_INCLUDED
#define STANDARDESE_OUTPUT_GENERATOR_HTML_HTML_GENERATOR_HPP_INCLUDED

#include "../../markup/entity.hpp"

namespace standardese::output::generator::html
{

class html_generator {
    public:

    static std::string render(const markup::entity&);
};

}

#endif

