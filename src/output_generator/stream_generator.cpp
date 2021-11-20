// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <stdexcept>
#include <ostream>

#include "../../standardese/output_generator/stream_generator.hpp"

namespace standardese::output_generator {

stream_generator::stream_generator(std::ostream* out): out(out) {
  if (out == nullptr)
    throw std::invalid_argument("output stream must not be NULL");
  if (out->bad())
    throw std::invalid_argument("output stream must be healthy");
}

stream_generator::~stream_generator() {}

}
