// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_MODEL_EXCLUDE_MODE_HPP_INCLUDED
#define STANDARDESE_MODEL_EXCLUDE_MODE_HPP_INCLUDED

namespace standardese::model
{

/// Whether parts of a C++ entity should be excluded from the generated documentation.
enum class exclude_mode {
    /// Do not exclude anything.
    include,
    /// This C++ entity had no standardese-comments. We might end up excluding it.
    uncommented,
    /// Exclude this entity completely.
    exclude,
    /// Hide the return type of this function in the output.
    exclude_return_type,
    /// Hide the target of this alias from the output.
    exclude_target,
};

}

#endif
