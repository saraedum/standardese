// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_MODEL_MIXIN_DOCUMENTATION_HPP
#define STANDARDESE_MODEL_MIXIN_DOCUMENTATION_HPP

#include <type_safe/optional.hpp>
#include <type_safe/reference.hpp>

#include <cppast/forward.hpp>

#include "anchored_container.hpp"
#include "../markup/code_block.hpp"
#include "../exclude_mode.hpp"

namespace standardese::model::mixin
{

/// Documentation for something in the C++ source code such as a file, a class,
/// a parameter, or a module.
class documentation : public anchored_container<entity>
{
public:
    using anchored_container<entity>::anchored_container;

    /// The explicit synopsis override if specified by the user.
    type_safe::optional<std::string> synopsis;

    /// Whether to exclude this entity or parts of it from the output.
    enum exclude_mode exclude_mode = exclude_mode::include;

    /// If set, this entity will appear grouped with all sibling that have the same group.
    type_safe::optional<std::string> group;

    /// If set, this entity will appear under this heading.
    type_safe::optional<std::string> output_section;

    /// If set, this entity will also show up in the module index for this module.
    type_safe::optional<std::string> module;
};

}

#endif
