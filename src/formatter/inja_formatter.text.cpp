// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <fmt/format.h>

#include "inja_formatter.impl.hpp"
#include "../../standardese/parser/markdown_parser.hpp"
#include "../../standardese/model/document.hpp"
#include "../../standardese/output_generator/text/text_generator.hpp"

namespace standardese::formatter {

std::string inja_formatter::text_callback(const nlohmann::json& data) const {
  const auto md = md_callback(data);
  return text(parser::markdown_parser{}.parse(md));
}

std::string inja_formatter::text(const model::document& document) const {
  return output_generator::text::text_generator::render(document);
}

}

