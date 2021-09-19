// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include "regex.hpp"

std::regex standardese::util::regex::inja_formatter_sanitize_basename_forbidden{R"((\W|_)+)"};
std::regex standardese::util::regex::sphinx_load_v2_zlib{"zlib"};
// Essentially the same as https://github.com/sphinx-doc/sphinx/blob/4.x/sphinx/util/inventory.py#L123
std::regex standardese::util::regex::sphinx_load_v2_syntax{R"((.+?)\s+(\S+)\s+(-?\d+)\s+?(\S*)\s+(.*)\s*)"};
std::regex standardese::util::regex::command_character_escaped_command("\\w");
std::regex standardese::util::regex::options_parser_process_legacy_comment_options_syntax{R"(([^=]*)=(.*\$\$.*))"};
std::regex standardese::util::regex::options_parser_process_external_options_syntax{"([^:]*):([^:]*):([^=]*)=(.*)"};
std::regex standardese::util::regex::options_parser_escape_inja_control{"[{}]"};
std::regex standardese::util::regex::anchor_transformation_strip(R"([^\w\s-])");
std::regex standardese::util::regex::anchor_transformation_escape(R"([-\s]+)");
std::regex standardese::util::regex::link_external_legacy_transformation_pattern{R"((?:::)?(([^:]*)::.*))"};
std::regex standardese::util::regex::link_external_legacy_transformation_replace{R"(\$\$)"};
std::regex standardese::util::regex::link_target_internal_transformation_entity_pattern{"^standardese://@([0-9]*)$"};
std::regex standardese::util::regex::link_target_internal_transformation_uri_pattern{R"(^(([^:/?#]+):)(//([^/?#]*))([^?#]*)(\?([^#]*))?(#(.*))?)"};
