// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include "../../standardese/inventory/unique_name_inventory.hpp"

namespace standardese::inventory {

unique_name_inventory::unique_name_inventory(const model::entity_set* entities, const cppast_inventory* inventory): entities(entities), inventory(inventory) {}

}
