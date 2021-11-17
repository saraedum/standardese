// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_OUTPUT_DOXYGEN_TAGFILE_GENERATOR_HPP_INCLUDED
#define STANDARDESE_OUTPUT_DOXYGEN_TAGFILE_GENERATOR_HPP_INCLUDED

#include <cppast/cppast_fwd.hpp>

#include "../stream_generator.hpp"
#include "../../inventory/doxygen/tagfile.hpp"

namespace standardese::output_generator::doxygen
{

class tagfile_generator : public stream_generator {
  public:
    /// Create a generator that writes a [Doxygen
    /// Tagfile](https://www.doxygen.nl/manual/external.html).
    /// Upon destruction of this object, the tagfile is written to [os]().
    tagfile_generator(std::ostream* os);

    void visit(document&) override;

    ~tagfile_generator() override;

  private:
    inventory::doxygen::tagfile tagfile;

    std::string path;
};

}

#endif


