// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <ostream>

#include "../../../standardese/output_generator/doxygen/tagfile_generator.hpp"
#include "../../../standardese/model/document.hpp"

namespace standardese::output_generator::doxygen {

tagfile_generator::tagfile_generator(std::ostream* os) : stream_generator(os) {}

tagfile_generator::~tagfile_generator() {
  *out << tagfile << std::flush;
}

void tagfile_generator::visit(document& document) {
  path = document.path;

  stream_generator::visit(document);
}

}
