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

namespace standardese::parser {

namespace
{

// Return command_character, e.g., '\', as something that can be used in a
// regular expression, e.g., '\\'.
std::string command_character_escaped(char command_character) {
  std::string escaped = " ";
  escaped[0] = command_character;
  if (!std::regex_match(escaped, std::regex("\\w"))) {
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

comment_parser::comment_parser_options::comment_parser_options(char command_character, const std::vector<std::string>& command_patterns) {
    const auto pattern = [&](const auto command) {
        const std::string name = command_name(command);
        const auto fallback = default_command_pattern(command_character, command);

        std::vector<std::string> parameters { name + "=" + fallback };

        for (const auto& specification : command_patterns)
            if (specification.rfind(name, 0) != std::string::npos)
                parameters.emplace_back(specification);

        return command_pattern(parameters);
    };

    this->command_extension_options.end_command_pattern = pattern(commands::special_command::end);
    this->command_extension_options.exclude_command_pattern = pattern(commands::special_command::exclude);
    this->command_extension_options.unique_name_command_pattern = pattern(commands::special_command::unique_name);
    this->command_extension_options.output_name_command_pattern = pattern(commands::special_command::output_name);
    this->command_extension_options.synopsis_command_pattern = pattern(commands::special_command::synopsis);
    this->command_extension_options.group_command_pattern = pattern(commands::special_command::group);
    this->command_extension_options.module_command_pattern = pattern(commands::special_command::module);
    this->command_extension_options.output_section_command_pattern = pattern(commands::special_command::output_section);
    this->command_extension_options.entity_command_pattern = pattern(commands::special_command::entity);
    this->command_extension_options.file_command_pattern = pattern(commands::special_command::file);

    this->command_extension_options.brief_command_pattern = pattern(commands::section_command::brief);
    this->command_extension_options.details_command_pattern = pattern(commands::section_command::details);
    this->command_extension_options.requires_command_pattern = pattern(commands::section_command::requires);
    this->command_extension_options.effects_command_pattern = pattern(commands::section_command::effects);
    this->command_extension_options.synchronization_command_pattern = pattern(commands::section_command::synchronization);
    this->command_extension_options.postconditions_command_pattern = pattern(commands::section_command::postconditions);
    this->command_extension_options.returns_command_pattern = pattern(commands::section_command::returns);
    this->command_extension_options.throws_command_pattern = pattern(commands::section_command::throws);
    this->command_extension_options.complexity_command_pattern = pattern(commands::section_command::complexity);
    this->command_extension_options.remarks_command_pattern = pattern(commands::section_command::remarks);
    this->command_extension_options.error_conditions_command_pattern = pattern(commands::section_command::error_conditions);
    this->command_extension_options.notes_command_pattern = pattern(commands::section_command::notes);
    this->command_extension_options.preconditions_command_pattern = pattern(commands::section_command::preconditions);
    this->command_extension_options.constraints_command_pattern = pattern(commands::section_command::constraints);
    this->command_extension_options.diagnostics_command_pattern = pattern(commands::section_command::diagnostics);
    this->command_extension_options.see_command_pattern = pattern(commands::section_command::see);
    this->command_extension_options.parameters_command_pattern = pattern(commands::section_command::parameters);
    this->command_extension_options.bases_command_pattern = pattern(commands::section_command::bases);

    this->command_extension_options.base_command_pattern = pattern(commands::inline_command::base);
    this->command_extension_options.param_command_pattern = pattern(commands::inline_command::param);
    this->command_extension_options.tparam_command_pattern = pattern(commands::inline_command::tparam);
}

}
