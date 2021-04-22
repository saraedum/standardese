// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <sstream>
#include <regex>

#include "../../standardese/transformation/anchor_transformation.hpp"
#include "../../standardese/model/mixin/anchored.hpp"
#include "../../standardese/model/unordered_entities.hpp"
#include "../../standardese/model/visitor/visit.hpp"

namespace standardese::transformation {

void anchor_transformation::do_transform(model::entity& document) {
  std::string path;

  model::visitor::visit([&](auto&& entity, auto&& recurse) {
    using T = std::decay_t<decltype(entity)>;
    if constexpr (std::is_base_of_v<model::document, T>) {
      // TODO
      // path = entity.path;
    } else if constexpr (std::is_base_of_v<model::mixin::anchored, T>) {
      // TODO: We are using knowledge about mkdocs here. Instead we should
      // offer several implementations here:
      // * render a preceding <a>
      // * render the first element manually as HTML and add an id
      // * Render some MarkDown extension {}
      // * Render for mkdocs/hugo/...

      // TODO: Anyway, what mkdocs does is not a bad strategy in general: https://github.com/Python-Markdown/markdown/blob/master/markdown/extensions/toc.py#L26
      
      // TODO: What can we do when there is no heading?
      if (entity.begin() != entity.end() && entity.begin()->template is<model::markup::heading>()) {
        auto& heading = entity.begin()->template as<model::markup::heading>();

        std::string inner;
        model::visitor::visit([&](auto&& node, auto&& recurse) {
          using T = std::decay_t<decltype(node)>;
          if constexpr (std::is_same_v<T, model::markup::text>) {
            inner += node.value;
          }
          recurse();
        }, heading);
        
        std::regex strip(R"([^\w\s-])");
        std::regex escape(R"([-\s]+)");
        inner = std::regex_replace(inner, strip, "");
        std::transform(inner.begin(), inner.end(), inner.begin(), [](unsigned char c){ return std::tolower(c); });
        inner = std::regex_replace(inner, escape, "-");

        entity.id = inner;
      }
    }

    recurse();
  }, document);
}

}
