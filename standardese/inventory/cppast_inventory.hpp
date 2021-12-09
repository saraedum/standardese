// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_INVENTORY_CPPAST_INVENTORY_HPP_INCLUDED
#define STANDARDESE_INVENTORY_CPPAST_INVENTORY_HPP_INCLUDED

#include <cppast/cppast_fwd.hpp>
#include <unordered_set>

#include "../parser/cpp_context.hpp"
#include "inventory.hpp"

namespace standardese::inventory
{

/// An inventory of symbols as parsed by cppast.
class cppast_inventory : public inventory {
  public:
    /// Create an inventory for the files which contain these entities.
    cppast_inventory(std::vector<const cppast::cpp_entity*>, const parser::cpp_context&);

    /// Create an inventory for the files which are represented by the `entities`.
    cppast_inventory(const model::entity_set* entities, const parser::cpp_context&);

    /// Lookup the symbol `name` relative to the `entity`, e.g., because
    /// `name` is mentioned in the comment for `entity`.
    /// Returns a null pointer when no such entity could be found.
    static const cppast::cpp_entity* find(const std::string& name, const symbols&, const cppast::cpp_entity&);

    /// Lookup the symbol `name`.
    /// Returns a null pointer when no such entity could be found.
    static const cppast::cpp_entity* find(const std::string& name, const symbols&);

  private:
    friend class symbols;

    static const cppast::cpp_file& root(const cppast::cpp_entity& entity_);

    std::unordered_set<const cppast::cpp_file*> roots;
    const parser::cpp_context context;
};

}

#endif
