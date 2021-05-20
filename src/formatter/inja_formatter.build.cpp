// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <fmt/format.h>

#include "../../standardese/formatter/inja_formatter.hpp"
#include "../../standardese/model/document.hpp"
#include "../../standardese/parser/markdown_parser.hpp"
#include "../../standardese/model/visitor/visit.hpp"
#include "../../standardese/logger.hpp"
#include "inja_formatter.impl.hpp"

namespace standardese::formatter {

model::document inja_formatter::build(const std::string &format) const {
  const std::string markdown = this->format(format);

  model::document parsed = parser::markdown_parser{}.parse(markdown);

  model::visitor::visit([&](auto& entity, auto&& recurse) {
    using T = std::decay_t<decltype(entity)>;

    if constexpr (std::is_same_v<T, model::markup::link>) {
      if (entity.target.href().has_value()) {
        std::string href = entity.target.href().value();

        if (href.rfind(self->href_schema) == 0)
          std::visit([&](auto&& parsed) {
            using T = std::decay_t<decltype(parsed)>;
            if constexpr (std::is_same_v<T, model::link_target>)
              entity.target = parsed;
            else
              logger::error(fmt::format("Could not parse link target `{}` which is not of a supported kind.", href));
          }, impl::from_json(nlohmann::json::parse(href.substr(self->href_schema.size()))));
      }
    }

    recurse();
  }, parsed);

  return parsed;
}

// TODO: Randomize
std::string inja_formatter::impl::href_schema = "json://";

}
