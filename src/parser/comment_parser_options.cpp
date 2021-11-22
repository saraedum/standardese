// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//               2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <cassert>
#include <string>

#include "../../standardese/parser/commands/special_command.hpp"
#include "../../standardese/parser/commands/section_command.hpp"
#include "../../standardese/parser/commands/inline_command.hpp"
#include "../../standardese/parser/comment_parser.hpp"
#include "../util/regex.hpp"

namespace standardese::parser {

namespace
{

// TODO(0.6.0-alpha): Move this into comment_parser.cpp

// Return command_character, e.g., '\', as something that can be used in a
// regular expression, e.g., '\\'.
std::string command_character_escaped(char command_character) {
  std::string escaped = " ";
  escaped[0] = command_character;
  if (!std::regex_match(escaped, util::regex::command_character_escaped_command)) {
    // Anything that is not alpha-numeric can be escaped with a backslash; it
    // probably does not need to be escaped though.
    escaped = "\\" + escaped;
  }
  return escaped;
}

const char* command_name(commands::special_command cmd) {
    switch(cmd) {
        case commands::special_command::output_name:
            return "output_name";
        case commands::special_command::output_section:
            return "output_section";
        case commands::special_command::end:
            return "end";
        case commands::special_command::entity:
            return "entity";
        case commands::special_command::exclude:
            return "exclude";
        case commands::special_command::file:
            return "file";
        case commands::special_command::group:
            return "group";
        case commands::special_command::module:
            return "module";
        case commands::special_command::synopsis:
            return "synopsis";
        case commands::special_command::unique_name:
            return "unique_name";
        default:
            throw std::logic_error("not implemented: unknown command type");
    }
}

const char* command_name(commands::section_command cmd) {
    switch(cmd) {
        case commands::section_command::brief:
            return "brief";
        case commands::section_command::details:
            return "details";
        case commands::section_command::requires:
            return "requires";
        case commands::section_command::effects:
            return "effects";
        case commands::section_command::synchronization:
            return "synchronization";
        case commands::section_command::postconditions:
            return "postconditions";
        case commands::section_command::returns:
            return "returns";
        case commands::section_command::throws:
            return "throws";
        case commands::section_command::complexity:
            return "complexity";
        case commands::section_command::remarks:
            return "remarks";
        case commands::section_command::error_conditions:
            return "error_conditions";
        case commands::section_command::notes:
            return "notes";
        case commands::section_command::preconditions:
            return "preconditions";
        case commands::section_command::constraints:
            return "constraints";
        case commands::section_command::diagnostics:
            return "diagnostics";
        case commands::section_command::see:
            return "see";
        case commands::section_command::parameters:
            return "parameters";
        case commands::section_command::bases:
            return "bases";
        default:
            throw std::logic_error("not implemented: unknown section type");
    }
}

const char* command_name(commands::inline_command cmd) {
    switch(cmd) {
        case commands::inline_command::base:
            return "base";
        case commands::inline_command::param:
            return "param";
        case commands::inline_command::tparam:
            return "tparam";
        default:
            throw std::logic_error("not implemented: unsupported inline type");
    }
}

std::regex command_pattern(const std::vector<std::string>& options)
{
    if (options.size() == 0)
        throw std::invalid_argument("expected at least one pattern to merge");

    std::vector<std::string> patterns;

    for (const auto& option : options) {
        const auto split = option.find('=');
        if (split == std::string::npos || split == 0)
            throw std::invalid_argument("argument must be of the form `name=pattern` but `" + option + "` is not.");

        const bool merge = option[split - 1] == '|';

        const auto pattern = option.substr(split + 1);

        if (!merge)
            patterns.clear();

        patterns.emplace_back(std::move(pattern));
    }

    assert(patterns.size() != 0);

    if (patterns.size() == 1)
        return std::regex(*begin(patterns));

    std::string combined;
    for (const auto& pattern : patterns) {
        if (combined.size() != 0)
            combined += "|";
        combined += "(?:" + pattern + ")";
    }

    return std::regex(combined);
}

const std::string eol = "(?:[[:space:]]*(?:\n|$))";
const std::string boundary = "(?:[[:space:]]+|" + eol + ")";
const std::string word = "[[:space:]]*([^[:space:]]+)" + boundary;
const std::string until_eol = "[[:space:]]*([^\n]*?)" + eol;

std::string default_command_pattern(char command_character, commands::special_command cmd)
{
    const std::string prefix = command_character_escaped(command_character);
    const std::string end = "(?:" + prefix + "end" + boundary + ")";

    const std::string name = command_name(cmd);

    switch (cmd)
    {
    case commands::special_command::end:
        return prefix + name + eol;
    case commands::special_command::exclude:
        return prefix + name + boundary + "(target|return)?" + eol;
    case commands::special_command::unique_name:
    case commands::special_command::output_name:
    case commands::special_command::module:
        return prefix + name + boundary + word + eol;
    case commands::special_command::output_section:
    case commands::special_command::entity:
    case commands::special_command::synopsis:
        return prefix + name + boundary + until_eol;
    case commands::special_command::group:
        return prefix + name + boundary + word + until_eol;
    case commands::special_command::file:
        return prefix + name + boundary;
    default:
        throw std::logic_error("not implemented: unknown command type");
    }
}

std::string default_command_pattern(char command_character, commands::section_command cmd)
{
    const std::string prefix = command_character_escaped(command_character);

    return prefix + command_name(cmd) + boundary;
}

std::string default_command_pattern(char command_character, commands::inline_command cmd)
{
    const std::string prefix = command_character_escaped(command_character);

    return prefix + command_name(cmd) + boundary + word;
}

}

comment_parser::comment_parser_options::comment_parser_options(char command_character, const std::vector<std::string>& command_patterns) : command_extension_options(command_character, command_patterns) {}

comment_parser::comment_parser_options::command_extension_options::command_extension_options(char command_character, const std::vector<std::string>& command_patterns) {
    const auto pattern = [&](const auto command) {
        const std::string name = command_name(command);
        const auto fallback = default_command_pattern(command_character, command);

        std::vector<std::string> parameters { name + "=" + fallback };

        for (const auto& specification : command_patterns)
            if (specification.rfind(name, 0) != std::string::npos)
                parameters.emplace_back(specification);

        return command_pattern(parameters);
    };

    end_command_pattern = pattern(commands::special_command::end);
    exclude_command_pattern = pattern(commands::special_command::exclude);
    unique_name_command_pattern = pattern(commands::special_command::unique_name);
    output_name_command_pattern = pattern(commands::special_command::output_name);
    synopsis_command_pattern = pattern(commands::special_command::synopsis);
    group_command_pattern = pattern(commands::special_command::group);
    module_command_pattern = pattern(commands::special_command::module);
    output_section_command_pattern = pattern(commands::special_command::output_section);
    entity_command_pattern = pattern(commands::special_command::entity);
    file_command_pattern = pattern(commands::special_command::file);

    brief_command_pattern = pattern(commands::section_command::brief);
    details_command_pattern = pattern(commands::section_command::details);
    requires_command_pattern = pattern(commands::section_command::requires);
    effects_command_pattern = pattern(commands::section_command::effects);
    synchronization_command_pattern = pattern(commands::section_command::synchronization);
    postconditions_command_pattern = pattern(commands::section_command::postconditions);
    returns_command_pattern = pattern(commands::section_command::returns);
    throws_command_pattern = pattern(commands::section_command::throws);
    complexity_command_pattern = pattern(commands::section_command::complexity);
    remarks_command_pattern = pattern(commands::section_command::remarks);
    error_conditions_command_pattern = pattern(commands::section_command::error_conditions);
    notes_command_pattern = pattern(commands::section_command::notes);
    preconditions_command_pattern = pattern(commands::section_command::preconditions);
    constraints_command_pattern = pattern(commands::section_command::constraints);
    diagnostics_command_pattern = pattern(commands::section_command::diagnostics);
    see_command_pattern = pattern(commands::section_command::see);
    parameters_command_pattern = pattern(commands::section_command::parameters);
    bases_command_pattern = pattern(commands::section_command::bases);

    base_command_pattern = pattern(commands::inline_command::base);
    param_command_pattern = pattern(commands::inline_command::param);
    tparam_command_pattern = pattern(commands::inline_command::tparam);
}

}
