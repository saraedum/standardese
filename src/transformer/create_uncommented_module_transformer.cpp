// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <vector>

#include "../../standardese/transformer/create_uncommented_module_transformer.hpp"
#include "../../standardese/model/entity.hpp"
#include "../../standardese/model/visitor/visit.hpp"
#include "../../standardese/model/unordered_entities.hpp"

namespace standardese::transformer {

std::vector<model::entity> create_uncommented_module_transformer::do_transform(const model::entity& document) const {
  std::vector<model::entity> modules;

  const auto ensure_module = [&](const std::string& name) {
    if (entities->find_module(name) == entities->end()) {
      auto module = model::module(name);
      module.exclude_mode = model::exclude_mode::uncommented;
      modules.push_back(std::move(module));
   }
  };

  model::visitor::visit([&](auto&& documentation) {
    using T = std::decay_t<decltype(documentation)>;
    if constexpr (std::is_same_v<T, model::cpp_entity_documentation>) {
      if (documentation.module)
        ensure_module(documentation.module.value());
    }
  }, document);

  return modules;
}

model::unordered_entities create_uncommented_module_transformer::merge(std::vector<std::vector<model::entity>>&& transformed) const {
  model::unordered_entities merged;

  for (auto& modules : transformed)
    for (auto& module : modules)
      if (merged.find_module(module.as<model::module>().name) == merged.end())
        merged.insert(module);

  return merged;
}

}
