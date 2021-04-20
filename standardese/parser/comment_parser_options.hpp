// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_PARSER_COMMENT_PARSER_OPTIONS_HPP_INCLUDED
#define STANDARDESE_PARSER_COMMENT_PARSER_OPTIONS_HPP_INCLUDED

#include <regex>
#include <vector>

namespace standardese::parser {

struct comment_parser_options {
  /// \param command_character form commands by prefixing this to the command name.
  ///
  /// \param command_patterns Override or complement command patterns with the ones given here.
  /// E.g., `brief=SUMMARY:` lets us write `SUMMARY:` instead of `\brief`.
  comment_parser_options(char command_character = '\\', const std::vector<std::string>& command_patterns = {});

  /// Whether commands should be applied to the entire file if they
  /// cannot be matched to another entity.
  bool free_file_comments = false;

  struct command_extension_options {
    std::regex end_command_pattern;
    std::regex exclude_command_pattern;
    std::regex unique_name_command_pattern;
    std::regex output_name_command_pattern;
    std::regex synopsis_command_pattern;
    std::regex group_command_pattern;
    std::regex module_command_pattern;
    std::regex output_section_command_pattern;
    std::regex entity_command_pattern;
    std::regex file_command_pattern;

    std::regex brief_command_pattern;
    std::regex details_command_pattern;
    std::regex requires_command_pattern;
    std::regex effects_command_pattern;
    std::regex synchronization_command_pattern;
    std::regex postconditions_command_pattern;
    std::regex returns_command_pattern;
    std::regex throws_command_pattern;
    std::regex complexity_command_pattern;
    std::regex remarks_command_pattern;
    std::regex error_conditions_command_pattern;
    std::regex notes_command_pattern;
    std::regex preconditions_command_pattern;
    std::regex constraints_command_pattern;
    std::regex diagnostics_command_pattern;
    std::regex see_command_pattern;
    std::regex parameters_command_pattern;
    std::regex bases_command_pattern;

    std::regex param_command_pattern;
    std::regex tparam_command_pattern;
    std::regex base_command_pattern;
  } command_extension_options;
};

}

#endif
