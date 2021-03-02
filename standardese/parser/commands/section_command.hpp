// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//               2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_PARSER_COMMANDS_SECTION_COMMAND_HPP_INCLUDED
#define STANDARDESE_PARSER_COMMANDS_SECTION_COMMAND_HPP_INCLUDED

#include "../../forward.hpp"

namespace standardese::parser::commands
{
    /// The type of a documentation section.
    enum class section_command
    {
        brief,
        details,

        // [structure.specifications]/3 sections
        requires,
        effects,
        synchronization,
        postconditions,
        returns,
        throws,
        complexity,
        remarks,
        error_conditions,
        notes,

        preconditions, //< For consistency with postconditions.

        // proposed by p0788, not including ensures and expects
        constraints, //< Compile-time requirements.
        diagnostics, //< Compile-time requirements that will yield error message
                     //(`static_assert()`).

        see,

        parameters,

        count,
    };
}

#endif // STANDARDESE_COMMENT_COMMANDS_HPP_INCLUDED


