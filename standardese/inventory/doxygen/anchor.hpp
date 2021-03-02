// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_INVENTORY_DOXYGEN_ANCHOR_HPP_INCLUDED
#define STANDARDESE_INVENTORY_DOXYGEN_ANCHOR_HPP_INCLUDED

#include <string>

#include "../../forward.hpp"

namespace standardese::inventory::doxygen {

// Corresponds to `TagAnchorInfo` in doxygen.
class anchor {
 public:
  std::string label;
  std::string filename;
  std::string title;
};

}

#endif

