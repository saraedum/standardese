// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_OUTPUT_XML_XML_GENERATOR_HPP_INCLUDED
#define STANDARDESE_OUTPUT_XML_XML_GENERATOR_HPP_INCLUDED

#include <pugixml.hpp>

#include "../stream_generator.hpp"

namespace standardese::output_generator::xml
{

/// Renders a tree of entities as XML.
class xml_generator : public stream_generator<xml_generator> {
  public:
    xml_generator(std::ostream&);

    virtual ~xml_generator();

    void visit(block_quote&) override;
    void visit(code&) override;
    void visit(code_block&) override;
    void visit(section&) override;
    void visit(emphasis&) override;
    void visit(cpp_entity_documentation&) override;
    void visit(hard_break&) override;
    void visit(heading&) override;
    void visit(link&) override;
    void visit(list_item&) override;
    void visit(module&) override;
    void visit(list&) override;
    void visit(paragraph&) override;
    void visit(soft_break&) override;
    void visit(strong_emphasis&) override;
    void visit(text&) override;
    void visit(thematic_break&) override;
    void visit(document&) override;
    void visit(image&) override;

  private:
    /// The root of the output XML document.
    pugi::xml_document xml_document;

    /// The XML node we are adding content to currently. Starts identical to
    /// document and is then changed as we recurse through the tree of entities.
    pugi::xml_node top;
};

}

#endif
