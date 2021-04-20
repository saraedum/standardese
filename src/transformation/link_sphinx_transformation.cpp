// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include "../../standardese/transformation/link_sphinx_transformation.hpp"
#include "../../standardese/model/visitor/recursive_visitor.hpp"
#include "../../standardese/model/markup/link.hpp"
#include "../../standardese/model/entity.hpp"

namespace standardese::transformation {

namespace {

// Visits all the entities in a document and turns links to Sphinx documentation into actual URLs.
struct visitor : model::visitor::recursive_visitor<false> {
  visitor(const inventory::sphinx::documentation_set& inventory, const std::string& url) : inventory(inventory), url(url) {}

  void visit(link& link) override;

  const inventory::sphinx::documentation_set& inventory;
  const std::string& url;
};

void visitor::visit(link& link) {
  link.target.accept([&](auto&& target) {
    using T = std::decay_t<decltype(target)>;
    if constexpr (std::is_same_v<T, model::link_target::sphinx_target>) {
      if (target.project == inventory.project && target.version == inventory.version)
        link.target = model::link_target::uri_target(url + target.entry.uri);
    }
  });

  model::visitor::recursive_visitor<false>::visit(link);
}

}

link_sphinx_transformation::link_sphinx_transformation(model::unordered_entities& documents, struct options options, inventory::sphinx::documentation_set inventory) : transformation(documents), options(std::move(options)), inventory(std::move(inventory)), target_transformation(documents, inventory::symbols(this->inventory)) {}

void link_sphinx_transformation::transform(threading::pool::factory workers) {
  target_transformation.transform(workers);
  transformation::transform(workers);
}

void link_sphinx_transformation::do_transform(model::entity& entity) {
  visitor visitor{inventory, options.url};
  entity.accept(visitor);
}

}
