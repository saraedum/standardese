// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include "../../standardese/transformation/link_href_sphinx_transformation.hpp"
#include "../../standardese/model/visitor/recursive_visitor.hpp"
#include "../../standardese/model/markup/link.hpp"
#include "../../standardese/model/entity.hpp"

namespace standardese::transformation {

namespace {

// Visits all the entities in a document and turns links to Sphinx documentation into actual URLs.
struct visitor : model::visitor::recursive_visitor<false> {
  visitor(const std::string& project, const std::string& version, const std::string& base_url) : project(project), version(version), base_url(base_url) {}

  void visit(link& link) override;

  const std::string& project;
  const std::string& version;
  const std::string& base_url;
};

}

link_href_sphinx_transformation::link_href_sphinx_transformation(model::unordered_entities& documents, const std::string& project, const std::string& version, const std::string& base_url) : transformation(documents), project(project), version(version), base_url(base_url) {}

void link_href_sphinx_transformation::do_transform(model::entity& entity) {
  visitor visitor{project, version, base_url};
  entity.accept(visitor);
}

namespace {

void visitor::visit(link& link) {
  link.target.accept([&](auto&& target) {
    using T = std::decay_t<decltype(target)>;
    if constexpr (std::is_same_v<T, model::link_target::sphinx_target>) {
      if (target.project == project && target.version == version)
        link.target = model::link_target::uri_target(base_url + target.entry.uri);
    }
  });

  model::visitor::recursive_visitor<false>::visit(link);
}

}

}
