// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_OUTPUT_STREAM_GENERATOR_HPP_INCLUDED
#define STANDARDESE_OUTPUT_STREAM_GENERATOR_HPP_INCLUDED

#include <stdexcept>
#include <sstream>
#include <string>

#include "../model/visitor/recursive_visitor.hpp"
#include "../model/entity.hpp"

namespace standardese::output_generator
{

class stream_generator : public model::visitor::recursive_visitor<true>
{
public:
    /// Create a generator that writes output to [os]() stream upon destruction.
    stream_generator(std::ostream* os) : out(os) {
      if (os == nullptr)
        throw std::invalid_argument("output stream must not be NULL");
      if (os->bad())
        throw std::invalid_argument("output stream must be healthy");
    }

    virtual ~stream_generator() {}

    template <typename G>
    static std::string render(const model::entity& root)
    {
        std::stringstream s;
        {
          auto generator = G(&s);
          root.accept(generator);
        }
        return s.str();
    }

protected:
    std::ostream* out;
};

}

#endif
