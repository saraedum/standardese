// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_INVENTORY_UNIQUE_NAME_INVENTORY_HPP_INCLUDED
#define STANDARDESE_INVENTORY_UNIQUE_NAME_INVENTORY_HPP_INCLUDED

#include "inventory.hpp"

namespace standardese::inventory
{

/// An inventory of symbols that have have been named by a `\unique_name` command.
class unique_name_inventory : public inventory {
  public:
    /// Create an inventory from the documentation entities.
    unique_name_inventory(const model::entity_set* entities, const cppast_inventory*);

  private:
    friend class symbols;

    const model::entity_set* entities;
    const cppast_inventory* inventory;
};

}

#endif

