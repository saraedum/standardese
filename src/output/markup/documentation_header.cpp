// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include "../include/standardese/output/markup/documentation_header.hpp"

namespace standardese::output::markup
{

documentation_header documentation_header::clone() const
{
    return documentation_header(markup::clone(heading()), module());
}

}
