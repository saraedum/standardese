// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

// TODO: Actually, we probably do not want to do this.
/*
#include <fmt/format.h>

#include "inja_formatter.impl.hpp"
#include "../../standardese/logger.hpp"
#include "../../standardese/parser/markdown_parser.hpp"
#include "../../standardese/model/document.hpp"
#include "../../standardese/model/markup/paragraph.hpp"
#include "../../standardese/model/markup/code.hpp"
#include "../../standardese/model/markup/code_block.hpp"
#include "../../standardese/model/markup/text.hpp"

// TODO: It feels that this contains a lot of repeated code somehow.

namespace standardese::formatter {

std::string inja_formatter::code_join_callback(const nlohmann::json& separator, const nlohmann::json& entities) const {
  return std::visit([&](auto&& sep, auto&& items) {
    using S = std::decay_t<decltype(sep)>;
    using T = std::decay_t<decltype(items)>;
    if constexpr (std::is_same_v<S, const nlohmann::json::string_t*>) {
      if constexpr (std::is_same_v<T, const nlohmann::json::array_t*>) {
        std::vector<std::string> strings;

        for (const auto& string : *items) {
          std::visit([&](auto&& str) {
            using STR = std::decay_t<decltype(str)>;
            if constexpr (std::is_same_v<STR, const nlohmann::json::string_t*>) {
              strings.push_back(*str);
            } else {
              logger::error(fmt::format("Cannot join {} with template callback `code_join`.", nlohmann::json(string)));
            }
          }, self->from_json(string));
        }

        return code_join(std::move(strings), *sep);
      } else {
        logger::error(fmt::format("Template callback `code_join` cannot be used with non-array {}.", nlohmann::to_string(entities)));
      }
    } else {
      logger::error(fmt::format("Template callback `code_join` cannot be used with separator {}.", nlohmann::to_string(separator)));
    }

    return std::string{};
  }, self->from_json(separator), self->from_json(entities));
}

std::string inja_formatter::code_join(std::vector<std::string> items, const std::string& separator) const {
  const auto parser = parser::markdown_parser{};

  if (items.begin() == items.end())
    return std::string{};

  const auto parsed_separator = parser.parse(separator);
  std::string string_separator;
  bool block_separator = false;

  if (parsed_separator.begin() == parsed_separator.end())
    string_separator = "";
  else {
    if (++parsed_separator.begin() != parsed_separator.end())
      logger::warn(fmt::format("code_join() can not handle separators that consist of more than one block. Trailing blocks in will be ignored in: {}", separator));

    if (parsed_separator.begin()->is<model::markup::paragraph>()) {
      const auto& paragraph = parsed_separator.begin()->as<model::markup::paragraph>();
      if (paragraph.begin() == paragraph.end())
        string_separator = "";
      else {
        if (++paragraph.begin() != paragraph.end())
          logger::warn(fmt::format("code_join() separator must consist of a single piece of code. Trailing items will be ignored in: {}", separator));
        if (!paragraph.begin()->is<model::markup::code>()) {
          logger::error(fmt::format("code_join() separator must be a single piece of code but it is not: {}", separator));
          return std::string{};
        }
        const auto& code = paragraph.begin()->as<model::markup::code>();
        if (code.begin() == code.end())
          // An empty piece of code ` ` encodes a single whitespace.
          string_separator = " ";
        else {
          assert(++code.begin() == code.end() && "model::markup::code must contain at most one text.");
          assert(code.begin()->is<model::markup::text>() && "model::markup::code must only contain text.");
          string_separator = code.begin()->as<model::markup::text>().value;
        }
      }
    } else if (parsed_separator.begin()->is<model::markup::code_block>()) {
      const auto& block = parsed_separator.begin()->as<model::markup::code_block>();
      block_separator = true;
      // TODO: Assert that there is exactly one child and that it is a text.
      string_separator = block.begin()->as<model::markup::text>().value;
    } else {
      logger::error(fmt::format("code_join() can only join with an empty string, code or a code block but separator is neither of those: {}", separator));
      return std::string{};
    }
  }

  std::vector<model::document> parsed;
  for (const auto& item : items)
    parsed.push_back(parser.parse(item));

  auto it = parsed.begin();
  auto joined = *it++;

  for (;it != parsed.end(); ++it) {
    const auto& item = *it;
    if (item.begin() == item.end()) {
      logger::error(fmt::format("code_join() can only join items if they have code on their boundary. Ignoring an item which does not: {}", md(item)));
      continue;
    }

    const auto& previous = *(it - 1);
    if (previous.begin() == previous.end()) {
      logger::error(fmt::format("code_join() can only join items if they have code on their boundary. Ignoring an item following one which does not: {}", md(previous)));
      continue;
    }

    // When asked to join a code block and code say
    // ```
    // block
    // ```
    // and `code`, we always join them as
    // ```
    // block
    // code
    // ```
    // and not as
    // ```
    // blockcode
    // ```
    // This is somewhat heuristical but when anything is a block we treat everything as a block.
    const bool previous_ends_in_code_block = previous.rbegin()->is<model::markup::code_block>();
    const bool item_begins_in_code_block = item.begin()->is<model::markup::code_block>();
    const bool connect_as_blocks = block_separator || previous_ends_in_code_block || item_begins_in_code_block;

    if (connect_as_blocks) {
      if (joined.rbegin()->is<model::markup::paragraph>()) {
        auto paragraph = joined.rbegin()->as<model::markup::paragraph>();
        // TODO: How do we pick the language here?
        joined.add_child(model::markup::code_block{"c++"});
        auto code_block = joined.rbegin()->as<model::markup::code_block>();
        if (paragraph.rbegin() == paragraph.rend()) {
          logger::warn(R"(code_join() can only join paragraph ending in code but this one is empty. Did you forget to reject("empty", ...) the list of items before joining them?)");
          code_block.add_child(model::markup::text{""});
        } else if (paragraph.rbegin()->is<model::markup::code>()) {
          auto code = paragraph.rbegin()->as<model::markup::code>();
          code_block.add_child(model::markup::text{code.begin()->as<model::markup::text>().value});
          paragraph.erase(--paragraph.end());
        } else {
          logger::error(fmt::format("code_join() can only join items that end in code or a code block but this is neither: {}", md(paragraph)));
        }
      }
    }

    auto* append_to = &*joined.rbegin();
    if (append_to->is<model::markup::code_block>()) {
      ;
    } else if (append_to->is<model::markup::paragraph>()) {
      auto& paragraph = append_to->as<model::markup::paragraph>();
      if (paragraph.begin() == paragraph.end()) {
        logger::error(R"(code_join() can only join code and code blocks but one item ends in an empty paragraph. Did you forget to reject("empty", ...) the list of items before joining them?)");
        break;
      }
      append_to = &*paragraph.rbegin();
      if (!append_to->is<model::markup::code>()) {
        logger::error(fmt::format("code_join() can only join items that end in code or a code block but this is not code: {}", md(*append_to)));
        break;
      }
    } else {
      logger::error(fmt::format("code_join() can only join items that end in code or a code block but this is neither: {}", md(*append_to)));
      break;
    }

    const auto append = [&](const std::string& text) {
      if (append_to->is<model::markup::code>()) {
        auto& code = append_to->as<model::markup::code>();
        if (code.begin() == code.end())
          // There is no empty code in MarkDown but only ` ` which encodes a
          // single whitespace.
          code.add_child(model::markup::text{" "});
        code.begin()->as<model::markup::text>().value += text;
      } else if (append_to->is<model::markup::code_block>()) {
        auto& code = append_to->as<model::markup::code_block>();
        if (code.begin() == code.end())
          code.add_child(model::markup::text{text});
        else
          code.begin()->as<model::markup::text>().value += text;
      } else {
        logger::error(fmt::format("code_join() can only join with code or a code block but this item is not of this kind: {}", md(*append_to)));
      }
    };

    // Add the separator.
    append(string_separator);

    auto children = *item.begin();
    if (children.is<model::markup::code_block>()) {
      // This item is a code block. Merge it with the preceding code block.
      const auto& code = children.as<model::markup::code_block>();
      if (code.begin() == code.end())
        continue;
      if (++code.begin() != code.end())
        logger::warn(fmt::format("code_join() can only contain at most one string of code. Will ignore trailing blocks in {}", md(code)));
      append(code.begin()->as<model::markup::text>().value);
    } else if (children.is<model::markup::paragraph>()) {
      // This item is a paragraph. Merge its initial code with the preceding
      // code/code block and add all the other children.
      const auto& paragraph = children.as<model::markup::paragraph>();
      auto child = paragraph.begin();
      if (child == paragraph.end() || !child->is<model::markup::code>()) {
        logger::error(fmt::format("code_join() can only join items that start with code. But this item does not: {}", md(*child)));
      } else {
        const auto& code = child->as<model::markup::code>();
        if (code.begin() == code.end())
          // There is no empty code in MarkDown but only ` ` which encodes a
          // single whitespace.
          append(" ");
        else {
          if (++code.begin() != code.end())
            logger::error(fmt::format("code must not contain more than one text child ignoring trailing children of {}", md(code)));
          append(code.begin()->as<model::markup::text>().value);
          child++;
        }
      }

      // Add all the other children. If we added to a code block, create a new
      // paragraph from the children, otherwise, append the items to the last
      // paragraph.
      for(;child != paragraph.end(); child++) {
        if (joined.rbegin()->is<model::markup::paragraph>())
          joined.add_child(model::markup::paragraph{});
        joined.rbegin()->as<model::markup::paragraph>().add_child(*child);
      }
    } else {
      logger::error(fmt::format("code_join() can only join paragraphs or code blocks not {}", md(children)));
      continue;
    }

    if (++item.begin() != item.end())
      logger::warn(fmt::format("code_join() can only join items consisting of at most one paragraph or code block. Ignoring trailing blocks in {}", md(item)));
  }

  if (joined.begin() == joined.end()) {
    logger::warn("code_join() did produce an block.");
    return std::string{};
  }
  if (++joined.begin() != joined.end())
    logger::warn(fmt::format("code_join() did produce more than one block. Will ignore the trailing blocks in {}", md(joined)));

  if (joined.begin()->is<model::markup::paragraph>()) {
    const auto& paragraph = joined.begin()->as<model::markup::paragraph>();
    assert(++paragraph.begin() == paragraph.end() && "paragraph created by join must consist of exactly one code");
    const auto& code = paragraph.begin()->as<model::markup::code>();
    return md(code);
  } else if (joined.begin()->is<model::markup::code_block>()) {
    return md(*joined.begin());
  } else {
    logger::warn(fmt::format("code_join() did not produce a paragraph or a code block but {}", md(joined)));
    return md(*joined.begin());
  }
}

}
*/
