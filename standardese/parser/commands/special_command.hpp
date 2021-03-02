// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//               2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_PARSER_COMMANDS_SPECIAL_COMMAND_HPP_INCLUDED
#define STANDARDESE_PARSER_COMMANDS_SPECIAL_COMMAND_HPP_INCLUDED

#include "../../forward.hpp"

namespace standardese::parser::commands
{
    /// The documentation special commands.
    enum class special_command
    {
        end,

        exclude,
        unique_name,
        output_name,
        synopsis,

        group,
        module,
        output_section,

        entity,
        file,

        count,
    };
}

#endif // STANDARDESE_COMMENT_COMMANDS_HPP_INCLUDED

