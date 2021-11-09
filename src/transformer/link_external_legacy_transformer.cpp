// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include "../../standardese/transformer/link_external_legacy_transformer.hpp"
#include "../../standardese/model/visitor/visit.hpp"
#include "../../standardese/model/markup/link.hpp"
#include "../../standardese/model/entity.hpp"
#include "../../standardese/logger.hpp"

#include "../util/regex.hpp"

namespace standardese::transformer {

link_external_legacy_transformer::link_external_legacy_transformer(model::unordered_entities& documents, struct options options): transformer(documents), options(options) {}

void link_external_legacy_transformer::do_transform(model::entity& document) {
  model::visitor::visit([&](auto&& link, auto&& recurse) {
    using T = std::decay_t<decltype(link)>;
    if constexpr (std::is_same_v<T, model::markup::link>) {
      link.target.accept([&](auto&& target) {
        using T = std::decay_t<decltype(target)>;
        if constexpr (std::is_same_v<T, model::link_target::standardese_target>) {

          std::smatch match;
          if (std::regex_match(target.target, match, util::regex::link_external_legacy_transformer_pattern) && match.str(2) == options.namspace) {
            link.target = model::link_target::uri_target(std::regex_replace(options.url, util::regex::link_external_legacy_transformer_replace, match.str(1)));
          }
        }
      });
    }

    recurse();
  }, document);
}

}
