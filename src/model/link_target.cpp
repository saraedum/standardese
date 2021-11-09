// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include "../../standardese/model/link_target.hpp"
#include <variant>

namespace standardese::model
{

link_target::link_target(std::string target) : target(standardese_target(std::move(target))) {}

link_target::link_target(module_target target) : target(std::move(target)) {}

link_target::link_target(const cppast::cpp_entity& entity) : target(cppast_target(entity)) {}

link_target::link_target(sphinx_target target) : target(std::move(target)) {}

link_target::link_target(uri_target target) : target(std::move(target)) {}

link_target::standardese_target::standardese_target(std::string target) : target(std::move(target)) {}

link_target::cppast_target::cppast_target(const cppast::cpp_entity& entity) : target(entity) {}

link_target::sphinx_target::sphinx_target(const inventory::sphinx::documentation_set& inventory, inventory::sphinx::entry entry) : project(inventory.project), version(inventory.version), entry(std::move(entry)) {}

link_target::module_target::module_target(std::string module) : module(std::move(module)) {}

link_target::uri_target::uri_target(std::string uri) : uri(std::move(uri)) {}

type_safe::optional<std::string> link_target::href() const {
  return accept([&](auto&& target) -> type_safe::optional<std::string> {
    using T = std::decay_t<decltype(target)>;
    if constexpr (std::is_same_v<T, uri_target>) {
      return target.uri;
    } else {
      return type_safe::nullopt;
    }
  });
}

}
