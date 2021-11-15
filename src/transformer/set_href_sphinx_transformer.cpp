// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include "../../standardese/transformer/set_href_sphinx_transformer.hpp"
#include "../../standardese/model/visitor/visit.hpp"
#include "../../standardese/model/markup/link.hpp"
#include "../../standardese/model/entity.hpp"

namespace standardese::transformer {

set_href_sphinx_transformer::set_href_sphinx_transformer(model::unordered_entities& documents, struct options options, inventory::sphinx::documentation_set inventory) : transformer(documents), options(std::move(options)), inventory(std::move(inventory)), target_transformer(documents, inventory::symbols(this->inventory)) {}

void set_href_sphinx_transformer::transform(threading::pool::factory workers) {
  target_transformer.transform(workers);
  transformer::transform(workers);
}

void set_href_sphinx_transformer::do_transform(model::entity& document) {
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
