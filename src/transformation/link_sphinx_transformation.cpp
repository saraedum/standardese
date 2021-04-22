// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include "../../standardese/transformation/link_sphinx_transformation.hpp"
#include "../../standardese/model/visitor/visit.hpp"
#include "../../standardese/model/markup/link.hpp"
#include "../../standardese/model/entity.hpp"

namespace standardese::transformation {

link_sphinx_transformation::link_sphinx_transformation(model::unordered_entities& documents, struct options options, inventory::sphinx::documentation_set inventory) : transformation(documents), options(std::move(options)), inventory(std::move(inventory)), target_transformation(documents, inventory::symbols(this->inventory)) {}

void link_sphinx_transformation::transform(threading::pool::factory workers) {
  target_transformation.transform(workers);
  transformation::transform(workers);
}

void link_sphinx_transformation::do_transform(model::entity& document) {
  model::visitor::visit([&](auto&& link, auto&& recurse) {
    using T = std::decay_t<decltype(link)>;
    if constexpr (std::is_same_v<T, model::markup::link>) {
      link.target.accept([&](auto&& target) {
        using T = std::decay_t<decltype(target)>;
        if constexpr (std::is_same_v<T, model::link_target::sphinx_target>) {
          if (target.project == inventory.project && target.version == inventory.version)
            link.target = model::link_target::uri_target(options.url + target.entry.uri);
        }
      });
    }

    recurse();
  }, document);
}

}
