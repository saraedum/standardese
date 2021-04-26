// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//               2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

// TODO: Do not use asserts anywhere. Be more error tolerant.
#include <cassert>
#include <cmark-gfm.h>
#include <cmark-gfm-extension_api.h>
#include <fmt/format.h>
#include <regex>
#include <boost/algorithm/string/trim.hpp>
#include <iostream> // TODO

#include "cmark-extension/cmark_extension.hpp"

#include "../../standardese/parser/markdown_parser.hpp"
#include "../../standardese/model/entity.hpp"
#include "../../standardese/model/document.hpp"
#include "../../standardese/model/markup/block_quote.hpp"
#include "../../standardese/model/markup/code.hpp"
#include "../../standardese/model/markup/emphasis.hpp"
#include "../../standardese/model/cpp_entity_documentation.hpp"
#include "../../standardese/model/markup/hard_break.hpp"
#include "../../standardese/model/markup/heading.hpp"
#include "../../standardese/model/markup/link.hpp"
#include "../../standardese/model/markup/list_item.hpp"
#include "../../standardese/model/markup/list.hpp"
#include "../../standardese/model/markup/paragraph.hpp"
#include "../../standardese/model/section.hpp"
#include "../../standardese/model/markup/soft_break.hpp"
#include "../../standardese/model/markup/strong_emphasis.hpp"
#include "../../standardese/model/markup/text.hpp"
#include "../../standardese/model/markup/thematic_break.hpp"
#include "../../standardese/model/markup/image.hpp"
#include "../../standardese/model/visitor/visit.hpp"
#include "../../standardese/model/document.hpp"
#include "../../standardese/logger.hpp"

namespace standardese::parser {

markdown_parser::~markdown_parser() {}

model::document markdown_parser::parse(const std::string& comment) const {
    // Create fresh parser with extensions to detect standardese commands.
    using unique_parser = unique_cmark<cmark_parser, cmark_parser_free_with_extensions>;
    auto parser = unique_parser(cmark_parser_new(CMARK_OPT_SMART));

    // Parse the comment into a tree of cmark nodes.
    cmark_parser_feed(parser.get(), comment.c_str(), comment.size());
    using unique_node = unique_cmark<cmark_node, cmark_node_free>;
    auto root = unique_node(cmark_parser_finish(parser.get()));

    model::document doc{"", ""};
    visit_children(root.get(), [&](cmark_node* child) { doc.add_child(parse(child)); });

    return doc;
}

// TODO: parse is a weird term here. We are not parsing anything really.
model::entity markdown_parser::parse(cmark_node* node) const
{
    const auto parse_into = [&](cmark_node* parent, auto&& container) -> model::entity {
      using T = typename std::decay_t<decltype(container)>::entity;
      visit_children(parent, [&](cmark_node* child) {
        auto parsed = parse(child);
        if constexpr (std::is_same_v<T, model::entity>) {
          container.add_child(parsed);
        } else {
          if (!parsed.is<T>())
            logger::error(fmt::format("Ignoring child node of unexpected type. Cannot add this MarkDown node here: `{}`", cmark_extension::cmark_extension::to_xml(child)));
          else
            container.add_child(parsed.as<T>());
        }
      });
      return container;
    };

    switch(cmark_node_get_type(node)) {
      case CMARK_NODE_PARAGRAPH:
        return parse_into(node, model::markup::paragraph());
      case CMARK_NODE_SOFTBREAK:
        assert(cmark_node_first_child(node) == nullptr && "softbreak is not supposed to have any child nodes");
        return model::markup::soft_break();
      case CMARK_NODE_TEXT:
        assert(cmark_node_first_child(node) == nullptr && "text is not supposed to have any child nodes");
        return model::markup::text(cmark_node_get_literal(node));
      case CMARK_NODE_CODE:
        assert(cmark_node_first_child(node) == nullptr && "code is not supposed to have any child nodes");
        return model::markup::code(cmark_node_get_literal(node));
      case CMARK_NODE_EMPH:
        return parse_into(node, model::markup::emphasis());
      case CMARK_NODE_STRONG:
        return parse_into(node, model::markup::strong_emphasis());
      case CMARK_NODE_LINK:
        {
          // TODO: Make much of this configurable.
          // TODO: Read the commonmark spec to make sure we're not doing anything stupid here.

          // TODO: It would be nice to do something like the following:
          //       * do a fuzzy lookup as the default
          //       * when there is standardese://* do a fuzzy lookup.
          //       * when there is standardese:// do an exact lookup
          //       * when there is standardese://kind/* do a fuzzy lookup with a fixed type.
          //       * when there is standardese://kind/ do a lookup with a fixed type.

          std::string target = std::string(cmark_node_get_url(node));
          std::string title = std::string(cmark_node_get_title(node));

          if (title == "" && target == "") {
            cmark_node* child = cmark_node_first_child(node);
            if (child != nullptr && cmark_node_last_child(node) == child && cmark_node_get_type(child) == CMARK_NODE_TEXT) {
                // For a link of the form `[text]()`, `text` is the target of this link with standardese syntax.
                target = cmark_node_get_literal(child);
                if (target.size() != 0 && (target[0] == '?' || target[0] == '*'))
                    cmark_node_set_literal(child, target.substr(1).c_str());
            }
            return parse_into(node, model::markup::link(target, title));
          } else if (target == "") {
            // For a link of the form `[text](<> "title")` the `title` that is
            // usually used for the tooltip is the target.
            target = title;
            title = "";
            return parse_into(node, model::markup::link(target, title));
          } else {
            // Otherwise, we assume that this is a standard MarkDown link
            // (though it might still be using a special schema such as
            // standardese://.)
            return parse_into(node, model::markup::link(model::link_target::uri_target(target), title));
          }
        }
      case CMARK_NODE_IMAGE:
        return parse_into(node, model::markup::image(cmark_node_get_url(node), cmark_node_get_title(node)));
      case CMARK_NODE_BLOCK_QUOTE:
        return parse_into(node, model::markup::block_quote());
      case CMARK_NODE_LIST:
        return parse_into(node, model::markup::list(cmark_node_get_list_type(node) == CMARK_ORDERED_LIST));
      case CMARK_NODE_ITEM:
        return parse_into(node, model::markup::list_item());
      case CMARK_NODE_CODE_BLOCK:
        return model::markup::code_block(cmark_node_get_fence_info(node), cmark_node_get_literal(node));
      case CMARK_NODE_HEADING:
        return parse_into(node, model::markup::heading(cmark_node_get_heading_level(node)));
      case CMARK_NODE_THEMATIC_BREAK:
        return model::markup::thematic_break();
    }

    throw std::logic_error("not implemented: unexpected CommonMark node type: " + std::string(cmark_node_get_type_string(node)) + " at " + cmark_extension::cmark_extension::to_xml(node));
}

void markdown_parser::cmark_parser_free_with_extensions(cmark_parser* parser)
{
  // TODO
  /*
    auto cur = cmark_parser_get_syntax_extensions(parser);
    while (cur)
    {
        cmark_syntax_extension_free(cmark_get_default_mem_allocator(),
                                    static_cast<cmark_syntax_extension*>(cur->data));
        cur = cur->next;
    }
    cmark_parser_free(parser);
    */
}

void markdown_parser::visit(cmark_node* root, std::function<cmark_node*(cmark_node*)> callback) const
{
    using unique_iter = unique_cmark<cmark_iter, cmark_iter_free>;
    auto iter = unique_iter(cmark_iter_new(root));

    while (true)
    {
        switch(cmark_iter_get_event_type(iter.get()))
        {
            case CMARK_EVENT_DONE:
                return;
            case CMARK_EVENT_EXIT:
            case CMARK_EVENT_NONE:
                cmark_iter_next(iter.get());
                break;
            case CMARK_EVENT_ENTER:
                cmark_node* current = cmark_iter_get_node(iter.get());
                cmark_node* next = callback(cmark_iter_get_node(iter.get()));
                if (next == nullptr) {
                  // Delete the node the iterator is pointing to and advance the iterator manually.
                  cmark_iter_reset(iter.get(), current, CMARK_EVENT_EXIT);
                  cmark_iter_next(iter.get());
                  cmark_node_free(current);
                  continue;
                } else {
                  cmark_iter_reset(iter.get(), next, CMARK_EVENT_ENTER);
                  cmark_iter_next(iter.get());
                }
                break;
        }
    }
}

void markdown_parser::visit_children(cmark_node* parent, std::function<void(cmark_node*)> callback) const
{
    for (cmark_node* child = cmark_node_first_child(parent); child != nullptr; child = cmark_node_next(child))
        callback(child);
}

std::string markdown_parser::escape(const std::string& input) {
  using unique_node = unique_cmark<cmark_node, cmark_node_free>;
  unique_node text{cmark_node_new(CMARK_NODE_TEXT)};
  cmark_extension::cmark_extension::cmark_node_set_literal(text.get(), input.c_str());

  using unique_string = unique_cmark<char, free>;
  unique_string escaped{cmark_render_commonmark(text.get(), CMARK_OPT_DEFAULT, 0)};

  // Whitespace cannot be properly escaped in MarkDown; there's not much we
  // can do about it but it's probably safest to remove excess whitespace.
  std::string trimmed = escaped.get();
  boost::algorithm::trim(trimmed);
  return trimmed;
}

}
