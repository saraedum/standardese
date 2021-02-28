// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//               2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_OUTPUT_MARKUP_DOC_SECTION_HPP_INCLUDED
#define STANDARDESE_OUTPUT_MARKUP_DOC_SECTION_HPP_INCLUDED

#include "../../comment/commands.hpp"
#include "entity.hpp"

namespace standardese::output::markup
{
    using section_type = comment::section_type;

    /// A section in an entity documentation.
    class doc_section : public entity
    {
    protected:
        doc_section() noexcept = default;
    };
}

#endif
