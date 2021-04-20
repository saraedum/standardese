// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <regex>

#include "../../standardese/transformation/link_external_legacy_transformation.hpp"
#include "../../standardese/model/visitor/recursive_visitor.hpp"
#include "../../standardese/model/markup/link.hpp"
#include "../../standardese/model/entity.hpp"

namespace standardese::transformation {

namespace {

struct visitor : model::visitor::recursive_visitor<false> {
  visitor(const std::string& namspace, const std::string& url) : namspace(namspace), url(url) {}

  void visit(link& link) override;

  const std::string& namspace;
  const std::string& url;
};

}

link_external_legacy_transformation::link_external_legacy_transformation(model::unordered_entities& documents, struct options options): transformation(documents), options(options) {}

void link_external_legacy_transformation::do_transform(model::entity& document) {
  visitor visitor{options.namspace, options.url};
  document.accept(visitor);
}

namespace {

void visitor::visit(link& link) {
  link.target.accept([&](auto&& target) {
    using T = std::decay_t<decltype(target)>;
    if constexpr (std::is_same_v<T, model::link_target::standardese_target>) {
      const static std::regex pattern{R"((?:::)?(([^:]*)::.*))"};

      std::smatch match;
      if (std::regex_match(target.target, match, pattern) && match[2] == namspace) {
        const static std::regex replace{R"(\$\$)"};
        link.target = model::link_target::uri_target(std::regex_replace(url, replace, match.str(1)));
      }
    }
  });
}

}

}
