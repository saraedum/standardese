// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <cppast/cpp_file.hpp>
#include <boost/filesystem/path.hpp>
#include <fmt/format.h>
#include <ostream>
#include <sstream>

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
#include "../../../standardese/logger.hpp"

// TODO(0.6.0-beta): Can we somehow disable the XML document type for error messages and testing?

namespace standardese::output_generator::xml
{

pugi::xml_node xml_generator::append_child(pugi::xml_node& parent, const std::string& name) {
  auto node = parent.append_child(name.c_str());

  if (node.empty())
    logger::error(fmt::format("Could not create {} node in doxygen tagfile output.", name));

  return node;
}

pugi::xml_node xml_generator::append_child(pugi::xml_node& parent, pugi::xml_node_type type) {
  auto node = parent.append_child(type);

  if (node.empty())
    logger::error("Could not create node in doxygen tagfile output.");

  return node;
}

void xml_generator::set_attribute(pugi::xml_node& node, const std::string& name, const std::string& value) {
  auto attribute = node.append_attribute(name.c_str());

  if (attribute.empty())
    logger::error(fmt::format("Could not create attribute {} with value {} in doxygen tagfile output.", name, value));

  if (attribute.set_value(value.c_str()) != static_cast<bool>(value.size()))
    logger::error(fmt::format("Could not set attribute {} to value {} in doxygen tagfile output.", name, value));
}

void xml_generator::set_attribute(pugi::xml_node& node, const std::string& name, int value) {
  auto attribute = node.append_attribute(name.c_str());

  if (attribute.empty())
    logger::error(fmt::format("Could not create attribute {} with value {} in doxygen tagfile output.", name, value));

  if (!attribute.set_value(value))
    logger::error(fmt::format("Could not set attribute {} to value {} in doxygen tagfile output.", name, value));
}

xml_generator::xml_generator(std::ostream* os, unsigned int format) : stream_generator(os), format(format), xml_document(), top(xml_document) {}

void xml_generator::visit(block_quote& block_quote) {
    top = append_child(top, "block-quote");
    stream_generator::visit(block_quote);
    top = top.parent();
}

void xml_generator::visit(code& code) {
    top = append_child(top, "code");
    stream_generator::visit(code);
    top = top.parent();
}

void xml_generator::visit(code_block& code_block) {
    top = append_child(top, "code-block");

    if (!code_block.language.empty())
        set_attribute(top, "language", code_block.language.c_str());

    stream_generator::visit(code_block);
    top = top.parent();
}

void xml_generator::visit(section& section) {
    top = append_child(top, "section");
    set_attribute(top, "name", [&]() {
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
    top = append_child(top, "emphasis");
    stream_generator::visit(emphasis);
    top = top.parent();
}

void xml_generator::visit(cpp_entity_documentation& entity_documentation) {
    top = append_child(top, "entity-documentation");

    std::string name = entity_documentation.entity().name();
    if (entity_documentation.entity().kind() == cppast::cpp_file::kind())
      name = boost::filesystem::path(name).filename().native();

    set_attribute(top, "name", name.c_str());

    if (entity_documentation.synopsis)
        set_attribute(top, "synopsis", entity_documentation.synopsis.value().c_str());

    stream_generator::visit(entity_documentation);
    top = top.parent();
}

void xml_generator::visit(group_documentation& group_documentation) {
    top = append_child(top, "group-documentation");

    if (group_documentation.synopsis)
        set_attribute(top, "synopsis", group_documentation.synopsis.value().c_str());

    stream_generator::visit(group_documentation);
    top = top.parent();
}

void xml_generator::visit(hard_break& hard_break) {
    top = append_child(top, "hard-break");
    stream_generator::visit(hard_break);
    top = top.parent();
}

void xml_generator::visit(heading& heading) {
    top = append_child(top, "heading");
    set_attribute(top, "level", heading.level);
    stream_generator::visit(heading);
    top = top.parent();
}

void xml_generator::visit(link& link) {
    top = append_child(top, "link");

    link.target.accept([&](auto&& target) -> void {
      using T = std::decay_t<decltype(target)>;
      if constexpr (std::is_same_v<T, model::link_target::standardese_target>) {
        set_attribute(top, "target", target.target.c_str());
      } else if constexpr (std::is_same_v<T, model::link_target::module_target>) {
        set_attribute(top, "target-module", target.module.c_str());
      } else if constexpr (std::is_same_v<T, model::link_target::cppast_target>) {
        set_attribute(top, "target-entity", target.target->name().c_str());
      } else if constexpr (std::is_same_v<T, model::link_target::uri_target>) {
        ;
      } else if constexpr (std::is_same_v<T, model::link_target::sphinx_target>) {
        set_attribute(top, "sphinx-target", target.entry.name.c_str());
      } else {
        throw std::logic_error("not implemented: cannot render this link target type yet");
      }
    });
    if (!link.title.empty())
      set_attribute(top, "title", link.title.c_str());
    if (link.target.href())
      set_attribute(top, "href", link.target.href().value().c_str());

    stream_generator::visit(link);
    top = top.parent();
}

void xml_generator::visit(list_item& list_item) {
    top = append_child(top, "list-item");
    stream_generator::visit(list_item);
    top = top.parent();
}

void xml_generator::visit(module& module) {
    top = append_child(top, "module");
    stream_generator::visit(module);
    top = top.parent();
}

void xml_generator::visit(list& list) {
    top = append_child(top, list.ordered ? "ordered-list" : "unordered-list");
    stream_generator::visit(list);
    top = top.parent();
}

void xml_generator::visit(paragraph& paragraph) {
    top = append_child(top, "paragraph");
    stream_generator::visit(paragraph);
    top = top.parent();
}

void xml_generator::visit(soft_break& soft_break) {
    top = append_child(top, "soft-break");
    stream_generator::visit(soft_break);
    top = top.parent();
}

void xml_generator::visit(strong_emphasis& strong_emphasis) {
    top = append_child(top, "strong");
    stream_generator::visit(strong_emphasis);
    top = top.parent();
}

void xml_generator::visit(text& text) {
    top = append_child(top, pugi::node_pcdata);
    top.text() = text.value.c_str();
    stream_generator::visit(text);
    top = top.parent();
}

void xml_generator::visit(thematic_break& thematic_break) {
    top = append_child(top, "thematic-break");
    stream_generator::visit(thematic_break);
    top = top.parent();
}

void xml_generator::visit(document& document) {
    top = append_child(top, "document");
    if (!document.name.empty())
      set_attribute(top, "name", document.name.c_str());
    stream_generator::visit(document);
    top = top.parent();
}

void xml_generator::visit(image& image) {
    top = append_child(top, "img");
    if (!image.src.empty())
      set_attribute(top, "src", image.src.c_str());
    if (!image.title.empty())
      set_attribute(top, "title", image.title.c_str());
    stream_generator::visit(image);
    top = top.parent();
}

std::string xml_generator::render(const model::entity& root) {
    std::stringstream s;
    {
      auto generator = xml_generator(&s);
      root.accept(generator);
    }
    return s.str();
}

xml_generator::~xml_generator() {
    xml_document.save(*out, "  ", format);
}

}
