// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <cassert>
#include <cmark-gfm.h>

#include "../../../standardese/output_generator/markdown/markdown_generator.hpp"
#include "../../../standardese/model/markup/code_block.hpp"
#include "../../../standardese/model/markup/heading.hpp"
#include "../../../standardese/model/markup/link.hpp"
#include "../../../standardese/model/markup/list.hpp"
#include "../../../standardese/model/markup/text.hpp"
#include "../../../standardese/model/markup/image.hpp"
#include "../../../standardese/model/cpp_entity_documentation.hpp"

namespace standardese::output_generator::markdown
{

namespace {

cmark_node* create_child(cmark_node* top, cmark_node_type type) {
  // TODO: Use the safe cmark wrapper from the extensions here.
  auto node = cmark_node_new(type);
  int success = cmark_node_append_child(top, node);
  assert(success);
  return node;
}

cmark_node* insert_child(cmark_node* top, cmark_node_type type) {
  // TODO: Use the safe cmark wrapper from the extensions here.
  auto node = cmark_node_new(type);
  int success = cmark_node_prepend_child(top, node);
  assert(success);
  return node;
}

}

markdown_generator::markdown_generator(std::ostream& os) : stream_generator<markdown_generator>(os), root(cmark_node_new(CMARK_NODE_DOCUMENT)), top(root.get()) {
  auto* node = cmark_node_new(CMARK_NODE_TEXT);
  cmark_node_set_literal(node, "HELLO");
  cmark_node_append_child(root.get(), node);
}

markdown_generator::~markdown_generator() {
  std::unique_ptr<char> str{cmark_render_commonmark(root.get(), CMARK_OPT_NOBREAKS, 0)};
  out_ << str.get();
}

void markdown_generator::visit(block_quote& block_quote) {
    top = create_child(top, CMARK_NODE_BLOCK_QUOTE);
    stream_generator::visit(block_quote);
    top = cmark_node_parent(top);
}

void markdown_generator::visit(code& code) {
    top = create_child(top, CMARK_NODE_CODE);
    stream_generator::visit(code);
    top = cmark_node_parent(top);
}

void markdown_generator::visit(code_block& code_block) {
    top = create_child(top, CMARK_NODE_CODE_BLOCK);

    if (!code_block.language.empty())
        cmark_node_set_fence_info(top, code_block.language.c_str());

    stream_generator::visit(code_block);
    top = cmark_node_parent(top);
}

void markdown_generator::visit(emphasis& emphasis) {
    top = create_child(top, CMARK_NODE_EMPH);
    stream_generator::visit(emphasis);
    top = cmark_node_parent(top);
}

void markdown_generator::visit(hard_break& hard_break) {
    create_child(top, CMARK_NODE_LINEBREAK);
}

void markdown_generator::visit(heading& heading) {
    top = create_child(top, CMARK_NODE_HEADING);

    cmark_node_set_heading_level(top, heading.level);

    stream_generator::visit(heading);
    top = cmark_node_parent(top);
}

void markdown_generator::visit(link& link) {
    top = create_child(top, CMARK_NODE_LINK);

    if (!link.title.empty())
      cmark_node_set_title(top, link.title.c_str());

    link.target.accept([&](auto&& target) -> void {
      using T = std::decay_t<decltype(target)>;
      if constexpr (std::is_same_v<T, model::link_target::standardese_target>) {
        cmark_node_set_url(top, target.target.c_str());
      } else if constexpr (std::is_same_v<T, model::link_target::sphinx_target>) {
        cmark_node_set_url(top, ("sphinx://" + target.project + "@" + target.entry.uri).c_str());
      } else if (link.target.href().has_value()) {
        cmark_node_set_url(top, link.target.href().value().c_str());
      } else {
        throw std::logic_error("not implemented: cannot render this kind of link as MarkDown");
      }
    });

    stream_generator::visit(link);
    top = cmark_node_parent(top);
}

void markdown_generator::visit(list_item& list_item) {
    top = create_child(top, CMARK_NODE_ITEM);
    stream_generator::visit(list_item);
    top = cmark_node_parent(top);
}

void markdown_generator::visit(list& list) {
    top = create_child(top, CMARK_NODE_LIST);

    cmark_node_set_list_type(top, list.ordered ? CMARK_ORDERED_LIST : CMARK_BULLET_LIST);

    stream_generator::visit(list);
    top = cmark_node_parent(top);
}

void markdown_generator::visit(paragraph& paragraph) {
    top = create_child(top, CMARK_NODE_PARAGRAPH);
    stream_generator::visit(paragraph);
    top = cmark_node_parent(top);
}

void markdown_generator::visit(soft_break& soft_break) {
    create_child(top, CMARK_NODE_SOFTBREAK);
}

void markdown_generator::visit(strong_emphasis& strong_emphasis) {
    top = create_child(top, CMARK_NODE_STRONG);
    stream_generator::visit(strong_emphasis);
    top = cmark_node_parent(top);
}

void markdown_generator::visit(cpp_entity_documentation& documentation) {
  stream_generator::visit(documentation);
}

void markdown_generator::visit(text& text) {
    if (cmark_node_get_type(top) == CMARK_NODE_CODE_BLOCK || cmark_node_get_type(top) == CMARK_NODE_CODE) {
        if (!cmark_node_get_literal(top))
            cmark_node_set_literal(top, "");
        cmark_node_set_literal(top, (cmark_node_get_literal(top) + text.value).c_str());
    } else {
      auto node = create_child(top, CMARK_NODE_TEXT);
      cmark_node_set_literal(node, text.value.c_str());
    }
}

void markdown_generator::visit(image& image) {
    top = create_child(top, CMARK_NODE_IMAGE);
    cmark_node_set_title(top, image.title.c_str());
    cmark_node_set_url(top, image.src.c_str());
    stream_generator::visit(image);
    top = cmark_node_parent(top);
}

void markdown_generator::visit(thematic_break& thematic_break) {
    create_child(top, CMARK_NODE_THEMATIC_BREAK);
}

}
