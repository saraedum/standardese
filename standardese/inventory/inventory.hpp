// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_INVENTORY_INVENTORY_HPP_INCLUDED
#define STANDARDESE_INVENTORY_INVENTORY_HPP_INCLUDED

#include <cppast/forward.hpp>
#include <string>

#include "../forward.hpp"

namespace standardese::inventory
{

class inventory {
 public:
  virtual ~inventory();

 protected:
  inventory();
};

}

#endif
