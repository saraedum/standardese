// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <regex>

#include "../../standardese/transformation/link_external_legacy_transformation.hpp"
#include "../../standardese/model/visitor/visit.hpp"
#include "../../standardese/model/markup/link.hpp"
#include "../../standardese/model/entity.hpp"
#include "../../standardese/logger.hpp"

namespace standardese::transformation {

link_external_legacy_transformation::link_external_legacy_transformation(model::unordered_entities& documents, struct options options): transformation(documents), options(options) {}

void link_external_legacy_transformation::do_transform(model::entity& document) {
  model::visitor::visit([&](auto&& link, auto&& recurse) {
    using T = std::decay_t<decltype(link)>;
    if constexpr (std::is_same_v<T, model::markup::link>) {
      link.target.accept([&](auto&& target) {
        using T = std::decay_t<decltype(target)>;
        if constexpr (std::is_same_v<T, model::link_target::standardese_target>) {
          const static std::regex pattern{R"((?:::)?(([^:]*)::.*))"};

          std::smatch match;
          if (std::regex_match(target.target, match, pattern) && match.str(2) == options.namspace) {
            const static std::regex replace{R"(\$\$)"};
            link.target = model::link_target::uri_target(std::regex_replace(options.url, replace, match.str(1)));
          }
        }
      });
    }

    recurse();
  }, document);
}

}
