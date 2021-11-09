// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_OUTPUT_MARKDOWN_MARKDOWN_GENERATOR_HPP_INCLUDED
#define STANDARDESE_OUTPUT_MARKDOWN_MARKDOWN_GENERATOR_HPP_INCLUDED

#include <cmark-gfm.h>

#include "../stream_generator.hpp"

namespace standardese::output_generator::markdown
{

class markdown_generator : public stream_generator<markdown_generator> {
  public:
    markdown_generator(std::ostream& os);

    void visit(block_quote&) override;
    void visit(code&) override;
    void visit(code_block&) override;
    void visit(emphasis&) override;
    void visit(hard_break&) override;
    void visit(heading&) override;
    void visit(link&) override;
    void visit(list_item&) override;
    void visit(list&) override;
    void visit(paragraph&) override;
    void visit(soft_break&) override;
    void visit(strong_emphasis&) override;
    void visit(text&) override;
    void visit(thematic_break&) override;
    void visit(cpp_entity_documentation&) override;
    void visit(image&) override;

    ~markdown_generator() override;

  private:
    std::unique_ptr<cmark_node, std::integral_constant<std::decay_t<decltype(cmark_node_free)>, cmark_node_free>> root;
    cmark_node* top;
};

}

#endif

