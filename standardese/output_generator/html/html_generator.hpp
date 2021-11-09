// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_OUTPUT_HTML_HTML_GENERATOR_HPP_INCLUDED
#define STANDARDESE_OUTPUT_HTML_HTML_GENERATOR_HPP_INCLUDED

#include "../stream_generator.hpp"

namespace standardese::output_generator::html
{

class html_generator : public stream_generator<html_generator> {
    public:
    html_generator(std::ostream& os);
};

}

#endif

