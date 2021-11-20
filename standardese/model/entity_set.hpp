// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_MODEL_ENTITIES_HPP_INCLUDED
#define STANDARDESE_MODEL_ENTITIES_HPP_INCLUDED

#include <cppast/cppast_fwd.hpp>
#include <boost/unordered_set.hpp>

#include "entity.hpp"

namespace standardese::model {

/// A hash function that identifies two entities if
/// * they are identical, or
/// * they are [cpp_entity_documentation]() for the same C++ entity, or
/// * they are [module_documentation]() for the same module.
struct cppast_entity_hash {
  size_t operator()(const entity&) const;
};

/// An equality function that identifies two entities if
/// * they are identical, or
/// * they are [cpp_entity_documentation]() for the same C++ entity, or
/// * they are [module_documentation]() for the same module.
struct cppast_entity_equality {
  bool operator()(const entity& lhs, const entity& rhs) const;
};

/// Insert [entity]() into this set.
/// A warning is produced if the entity cannot be added because it is already present.
/// However, some trivial entities are silently replaced.
void entity_set_insert(entity_set&, const entity& entity);
void entity_set_insert(entity_set&, entity&& entity);

/// Insert a range of entities into this set.
/// A warning is produced for the entities cannot be added, see
/// [cpp_entity_insert]().
template <typename Range>
void entity_set_extend(entity_set& entities, Range&& range) {
  for (auto& entity: range) {
    if constexpr (std::is_same_v<std::decay_t<decltype(entity)>, class entity>)
      entity_set_insert(entities, std::move(entity));
    else
      entity_set_extend(entities, entity);
  }
}

/// Return an iterator to the [cpp_entity_documentation]() documenting this C++ entity.
entity_set::const_iterator entity_set_find(const entity_set&, const cppast::cpp_entity&);
entity_set::iterator entity_set_find(entity_set&, const cppast::cpp_entity&);

/// Return an iterator to the [module_documentation]() documenting this module.
entity_set::const_iterator entity_set_find_module(const entity_set&, const std::string& name);
entity_set::iterator entity_set_find_module(entity_set&, const std::string& name);

/// Return the [cpp_entity_documentation]() documenting this C++ entity.
/// Throws an [std::invalid_argument]() exception if no such entity exists.
const entity& entity_set_at(const entity_set&, const cppast::cpp_entity&);
entity& entity_set_at(entity_set&, const cppast::cpp_entity&);

}

#endif
