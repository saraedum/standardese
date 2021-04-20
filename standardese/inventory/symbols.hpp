// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_INVENTORY_SYMBOLS_HPP_INCLUDED
#define STANDARDESE_INVENTORY_SYMBOLS_HPP_INCLUDED

#include "../parser/cpp_context.hpp"
#include "inventory.hpp"
#include "../model/link_target.hpp"

#include <memory>
#include <cppast/forward.hpp>
#include <type_safe/optional_ref.hpp>
#include <unordered_set>

namespace standardese::inventory
{

/// Mimicks some aspects of C++ name lookup.
/// This is used to determine the targets of `\entity` commands and to resolve
/// MarkDown links to C++ entities.
/// TODO: Make this aspect configurable:
/// Note that we are very liberal in our name lookup here. If there are
/// multiple symbols that match we just return the first of them. If that's
/// not the one that the user wanted, they can be completely specific
/// by using a `::full::global<T>::name<S>(int,char)`.
/// However, there are some things that cannot be distinguished because we are
/// so liberal here (and because C++ is an incredibly difficult language to
/// parse.) To distinguish these, we need to use `\unique_name` which is not
/// handled by this class at all.
class symbols {
 public:
  // TODO: Do we need to keep the inventory alive?
  explicit symbols(const inventory&);

  symbols(symbols&&);

  ~symbols();

  // TODO: Maybe merge these two and add more optional args: entity, policy (i.e., actual lookup options), kind.

  /// Lookup the symbol `name` relative to the `entity`, e.g., because
  /// `name` is mentioned in the comment for `entity`.
  type_safe::optional<model::link_target> find(const std::string& name, const cppast::cpp_entity& entity) const;

  /// Lookup the global symbol `name`.
  type_safe::optional<model::link_target> find(const std::string& name) const;

 private:
  struct impl;

  std::unique_ptr<struct impl> self;
};

}

#endif
