// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <cppast/cpp_file.hpp>
#include <boost/filesystem/path.hpp>

#include "../../../standardese/output_generator/xml/xml_generator.hpp"

#include "../../../standardese/model/markup/text.hpp"
#include "../../../standardese/model/markup/link.hpp"
#include "../../../standardese/model/markup/list.hpp"
#include "../../../standardese/model/markup/code_block.hpp"
#include "../../../standardese/model/markup/image.hpp"
#include "../../../standardese/model/section.hpp"
#include "../../../standardese/model/markup/heading.hpp"
#include "../../../standardese/model/document.hpp"
#include "../../../standardese/model/cpp_entity_documentation.hpp"
#include "../../../standardese/model/group_documentation.hpp"

// TODO: Can we somehow disable the XML document type for error messages and testing?

namespace standardese::output_generator::xml
{

xml_generator::xml_generator(std::ostream& os) : stream_generator(os), xml_document(), top(xml_document) {}

void xml_generator::visit(block_quote& block_quote) {
    top = top.append_child("block-quote");
    stream_generator::visit(block_quote);
    top = top.parent();
}

void xml_generator::visit(code& code) {
    top = top.append_child("code");
    stream_generator::visit(code);
    top = top.parent();
}

void xml_generator::visit(code_block& code_block) {
    top = top.append_child("code-block");

    if (!code_block.language.empty())
        top.append_attribute("language").set_value(code_block.language.c_str());
        
    stream_generator::visit(code_block);
    top = top.parent();
}

void xml_generator::visit(section& section) {
    top = top.append_child("section");
    top.append_attribute("name").set_value([&]() {
      switch (section.type) {
        case parser::commands::section_command::brief:
          return "Brief";
        case parser::commands::section_command::details:
          return "Details";
        case parser::commands::section_command::requires:
          return "Requires";
        case parser::commands::section_command::effects:
          return "Effects";
        case parser::commands::section_command::synchronization:
          return "Synchronization";
        case parser::commands::section_command::postconditions:
          return "Postconditions";
        case parser::commands::section_command::returns:
          return "Return values";
        case parser::commands::section_command::throws:
          return "Throws";
        case parser::commands::section_command::complexity:
          return "Complexity";
        case parser::commands::section_command::remarks:
          return "Remarks";
        case parser::commands::section_command::error_conditions:
          return "Error conditions";
        case parser::commands::section_command::notes:
          return "Notes";
        case parser::commands::section_command::preconditions:
          return "Preconditions";
        case parser::commands::section_command::constraints:
          return "Constraints";
        case parser::commands::section_command::diagnostics:
          return "Diagnostics";
        case parser::commands::section_command::see:
          return "See also";
        case parser::commands::section_command::parameters:
          return "Parameters";
        case parser::commands::section_command::bases:
          return "Base classes";
        default:
          throw std::logic_error("not implemented");
      }
    }());
    stream_generator::visit(section);
    top = top.parent();
}

void xml_generator::visit(emphasis& emphasis) {
    top = top.append_child("emphasis");
    stream_generator::visit(emphasis);
    top = top.parent();
}

void xml_generator::visit(cpp_entity_documentation& entity_documentation) {
    top = top.append_child("entity-documentation");

    std::string name = entity_documentation.entity().name();
    if (entity_documentation.entity().kind() == cppast::cpp_file::kind())
      name = boost::filesystem::path(name).filename().native();

    top.append_attribute("name").set_value(name.c_str());

    if (entity_documentation.synopsis)
        top.append_attribute("synopsis").set_value(entity_documentation.synopsis.value().c_str());

    stream_generator::visit(entity_documentation);
    top = top.parent();
}

void xml_generator::visit(group_documentation& group_documentation) {
    top = top.append_child("group-documentation");

    if (group_documentation.synopsis)
        top.append_attribute("synopsis").set_value(group_documentation.synopsis.value().c_str());

    stream_generator::visit(group_documentation);
    top = top.parent();
}

void xml_generator::visit(hard_break& hard_break) {
    top = top.append_child("hard-break");
    stream_generator::visit(hard_break);
    top = top.parent();
}

void xml_generator::visit(heading& heading) {
    top = top.append_child("heading");
    top.append_attribute("level").set_value(heading.level);
    stream_generator::visit(heading);
    top = top.parent();
}

void xml_generator::visit(link& link) {
    top = top.append_child("link");

    link.target.accept([&](auto&& target) -> void {
      using T = std::decay_t<decltype(target)>;
      if constexpr (std::is_same_v<T, model::link_target::standardese_target>) {
        top.append_attribute("target").set_value(target.target.c_str());
      } else if constexpr (std::is_same_v<T, model::link_target::module_target>) {
        top.append_attribute("target-module").set_value(target.module.c_str());
      } else if constexpr (std::is_same_v<T, model::link_target::cppast_target>) {
        top.append_attribute("target-entity").set_value(target.target->name().c_str());
      } else if constexpr (std::is_same_v<T, model::link_target::uri_target>) {
        ;
      } else if constexpr (std::is_same_v<T, model::link_target::sphinx_target>) {
        top.append_attribute("sphinx-target").set_value(target.entry.name.c_str());
      } else {
        throw std::logic_error("not implemented: cannot render this link target type yet");
      }
    });
    if (!link.title.empty())
      top.append_attribute("title").set_value(link.title.c_str());
    if (link.target.href())
      top.append_attribute("href").set_value(link.target.href().value().c_str());

    stream_generator::visit(link);
    top = top.parent();
}

void xml_generator::visit(list_item& list_item) {
    top = top.append_child("list-item");
    stream_generator::visit(list_item);
    top = top.parent();
}

void xml_generator::visit(module& module) {
    top = top.append_child("module");
    stream_generator::visit(module);
    top = top.parent();
}

void xml_generator::visit(list& list) {
    top = top.append_child(list.ordered ? "ordered-list" : "unordered-list");
    stream_generator::visit(list);
    top = top.parent();
}

void xml_generator::visit(paragraph& paragraph) {
    top = top.append_child("paragraph");
    stream_generator::visit(paragraph);
    top = top.parent();
}

void xml_generator::visit(soft_break& soft_break) {
    top = top.append_child("soft-break");
    stream_generator::visit(soft_break);
    top = top.parent();
}

void xml_generator::visit(strong_emphasis& strong_emphasis) {
    top = top.append_child("strong");
    stream_generator::visit(strong_emphasis);
    top = top.parent();
}

void xml_generator::visit(text& text) {
    top = top.append_child(pugi::node_pcdata);
    top.text() = text.value.c_str();
    stream_generator::visit(text);
    top = top.parent();
}

void xml_generator::visit(thematic_break& thematic_break) {
    top = top.append_child("thematic-break");
    stream_generator::visit(thematic_break);
    top = top.parent();
}

void xml_generator::visit(document& document) {
    top = top.append_child("document");
    if (!document.name.empty())
      top.append_attribute("name").set_value(document.name.c_str());
    stream_generator::visit(document);
    top = top.parent();
}

void xml_generator::visit(image& image) {
    top = top.append_child("img");
    if (!image.src.empty())
      top.append_attribute("src").set_value(image.src.c_str());
    if (!image.title.empty())
      top.append_attribute("title").set_value(image.title.c_str());
    stream_generator::visit(image);
    top = top.parent();
}

std::string xml_generator::render(const model::entity& root) {
  return stream_generator::render<xml_generator>(root);
}

xml_generator::~xml_generator() {
    xml_document.save(out_, "  ");
}

}
