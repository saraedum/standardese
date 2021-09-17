// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <fmt/format.h>

#include "inja_formatter.impl.hpp"
#include "../../standardese/parser/markdown_parser.hpp"
#include "../../standardese/model/document.hpp"
#include "../../standardese/output_generator/text/text_generator.hpp"
#include "../../standardese/logger.hpp"

namespace standardese::formatter {

std::string inja_formatter::text_callback(const nlohmann::json& data) const {
  return std::visit([&](auto&& entity) {
    using T = std::decay_t<decltype(entity)>;
    if constexpr (std::is_same_v<T, const nlohmann::json::string_t*>) {
      return text(static_cast<std::string>(data));
    } else {
      return text(md_callback(data));
    }
  }, self->from_json(data));
}

std::string inja_formatter::text(const model::entity& document) const {
  return output_generator::text::text_generator::render(document);
}

std::string inja_formatter::text(const std::string& markdown) const {
  auto text = this->text(parse(markdown));
  // TODO: Is this really the best way we can handle the fact that a newline is added by an implicitly created paragraph?
  if (text.size() && markdown.size() && *markdown.rbegin() != '\n')
    text.pop_back();
  return text;
}

}
