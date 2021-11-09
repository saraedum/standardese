// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <sstream>

#include "../../standardese/transformation/anchor_transformation.hpp"
#include "../../standardese/model/mixin/anchored.hpp"
#include "../../standardese/model/unordered_entities.hpp"
#include "../../standardese/model/visitor/visit.hpp"
#include "../util/regex.hpp"

namespace standardese::transformation {

void anchor_transformation::do_transform(model::entity& document) {
  std::string path;

  model::visitor::visit([&](auto&& entity, auto&& recurse) {
    using T = std::decay_t<decltype(entity)>;
    if constexpr (std::is_base_of_v<model::document, T>) {
      // TODO(0.6.0-alpha): Implement me.
      // path = entity.path;
    } else if constexpr (std::is_base_of_v<model::mixin::anchored, T>) {
      // TODO(0.6.0-beta): We are using knowledge about mkdocs here. Instead we should
      // offer several implementations here:
      // * render a preceding <a>
      // * render the first element manually as HTML and add an id
      // * Render some MarkDown extension {}
      // * Render for mkdocs/hugo/...

      // TODO(0.6.0-beta): Anyway, what mkdocs does is not a bad strategy in general: https://github.com/Python-Markdown/markdown/blob/master/markdown/extensions/toc.py#L26

      // TODO(0.6.0-beta): Complain when the anchor is not unique (and offer a solution?)

      // TODO(0.6.0-beta): What can we do when there is no heading?
      if (entity.children.begin() != entity.children.end() && entity.children.begin()->template is<model::markup::heading>()) {
        auto& heading = entity.children.begin()->template as<model::markup::heading>();

        std::string inner;
        model::visitor::visit([&](auto&& node, auto&& recurse) {
          using T = std::decay_t<decltype(node)>;
          if constexpr (std::is_same_v<T, model::markup::text>) {
            inner += node.value;
          }
          recurse();
        }, heading);

        inner = std::regex_replace(inner, util::regex::anchor_transformation_strip, "");
        std::transform(inner.begin(), inner.end(), inner.begin(), [](unsigned char c){ return std::tolower(c); });
        inner = std::regex_replace(inner, util::regex::anchor_transformation_escape, "-");
        // TODO(0.6.0-beta): Additionally, mkdocs sometimes adds _number to make things unique.

        entity.id = inner;
      }
    }

    recurse();
  }, document);
}

}
