// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <fmt/format.h>

#include "inja_formatter.impl.hpp"
#include "../../standardese/logger.hpp"
#include "../../standardese/output_generator/markdown/markdown_generator.hpp"
#include "../../standardese/model/markup/link.hpp"
#include "../../standardese/model/markup/paragraph.hpp"
#include "../../standardese/model/document.hpp"
#include "../../standardese/model/markup/block_quote.hpp"
#include "../../standardese/model/markup/list.hpp"
#include "../../standardese/model/markup/heading.hpp"
#include "../../standardese/model/markup/code_block.hpp"

namespace standardese::formatter {

std::string inja_formatter::md_callback(const nlohmann::json& data) const {
  return std::visit([&](auto&& entity) {
    using T = std::decay_t<decltype(entity)>;
    if constexpr (std::is_same_v<T, const model::entity*>) {
      return md(*entity);
    }

    logger::error(fmt::format("Cannot render `{}` as MarkDown in inja callback `md`.", self->to_string(data)));
    return std::string{};
  }, self->from_json(data));
}

std::string inja_formatter::md(const model::entity& entity) const {
  struct serialization_generator : output_generator::markdown::markdown_generator {
    serialization_generator(std::ostream& os, const inja_formatter& self) : markdown_generator(os), self(self) {}

    void visit(link& link) override {
      auto serializable = link;

      serializable.target.accept([&](auto&& target) -> void {
        using T = std::decay_t<decltype(target)>;
        if constexpr (std::is_same_v<T, model::link_target::cppast_target>) {
          serializable.target = model::link_target::uri_target(self.target(*target.target));
        }
      });

      markdown_generator::visit(serializable);
    }

    const inja_formatter& self;
  };

  bool wrapped_in_block;

  std::stringstream stream;
  {
    auto generator = serialization_generator(stream, *this);

    if (entity.is<model::document>() ||

        entity.is<model::markup::paragraph>() ||
        entity.is<model::markup::block_quote>() ||
        entity.is<model::markup::list>() ||
        entity.is<model::markup::heading>() ||
        // TODO: We really want to handle a section like an unnamed container when converting to md?
        entity.is<model::section>() ||
        entity.is<model::markup::code_block>()) {
      wrapped_in_block = false;
      entity.accept(generator);
    } else {
      wrapped_in_block = true;
      model::markup::paragraph paragraph{entity};
      paragraph.accept(generator);
    }
  }

  std::string markdown = stream.str();

  assert(markdown.size() && markdown.back() == '\n' && "MarkDown did not render with trailing newline");

  if (wrapped_in_block)
    // Drop the trailing newline that was inserted by wrapping in e.g. a paragraph.
    markdown.pop_back();

  return markdown;
}

}
