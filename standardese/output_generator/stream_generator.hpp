// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_OUTPUT_STREAM_GENERATOR_HPP_INCLUDED
#define STANDARDESE_OUTPUT_STREAM_GENERATOR_HPP_INCLUDED

#include <iosfwd>

#include "../model/visitor/recursive_visitor.hpp"

namespace standardese::output_generator
{

class stream_generator : public model::visitor::recursive_visitor<true>
{
public:
    /// Create a generator that writes output to [out]() stream upon destruction.
    stream_generator(std::ostream* out);

    /// Implementing class should override the destructor to actually write
    /// something to [out]().
    virtual ~stream_generator();

protected:
    std::ostream* out;
};

}

#endif
