// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <regex>

namespace standardese::util {

/// A collection of regular expressions that are used in the various areas of standardese.
/// Since non-default instantiation of a regex object is extremely costly to
/// compile time, we move all such instantiations to this shared place.
struct regex {
  static std::regex inja_formatter_sanitize_basename_forbidden;
  static std::regex sphinx_load_v2_zlib;

  static std::regex sphinx_load_v2_syntax;
  static std::regex command_character_escaped_command;
  static std::regex options_parser_process_legacy_comment_options_syntax;
  static std::regex options_parser_process_external_options_syntax;
  static std::regex options_parser_escape_inja_control;
  static std::regex options_parser_process_parser_options_macro;
  static std::regex anchor_transformer_strip;
  static std::regex anchor_transformer_escape;
  static std::regex link_external_legacy_transformer_pattern;
  static std::regex link_external_legacy_transformer_replace;
  static std::regex link_target_internal_transformer_entity_pattern;
  static std::regex link_target_internal_transformer_uri_pattern;
};

}
