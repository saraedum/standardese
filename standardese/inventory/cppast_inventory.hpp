// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_INVENTORY_CPPAST_INVENTORY_HPP_INCLUDED
#define STANDARDESE_INVENTORY_CPPAST_INVENTORY_HPP_INCLUDED

#include <cppast/forward.hpp>
#include <type_safe/optional_ref.hpp>
#include <unordered_set>

#include "../parser/cpp_context.hpp"
#include "inventory.hpp"

// TODO: Normalize member variables everywhere. Since we are not hiding all
// variables as private anymore, we should remove all the trailing _ and
// announce it as news.

namespace standardese::inventory
{

/// An inventory of symbols as parsed by cppast.
class cppast_inventory : public inventory {
  public:
    /// Create an inventory for the files which contain these entities.
    explicit cppast_inventory(std::vector<const cppast::cpp_entity*>, const parser::cpp_context&);

    /// Lookup the symbol `name` relative to the `entity`, e.g., because
    /// `name` is mentioned in the comment for `entity`.
    static type_safe::optional_ref<const cppast::cpp_entity> find(const std::string& name, const cppast::cpp_entity&, const parser::cpp_context&);

    static const cppast::cpp_file& root(const cppast::cpp_entity& entity_);

  private:
    friend class symbols;

    std::unordered_set<const cppast::cpp_file*> roots;
    const parser::cpp_context context;
};

}

#endif
