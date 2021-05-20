// Copyright (C) 2017 Jonathan Müller <jonathanmueller.dev@gmail.com>
//               2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_FORMATTER_SYNOPSIS_FORMATTER_HPP_INCLUDED
#define STANDARDESE_FORMATTER_SYNOPSIS_FORMATTER_HPP_INCLUDED

#include <string>

#include "inja_formatter.hpp"

namespace standardese::formatter {

/// Produces a synopsis for a documented C++ entity.
/// Essentially, this is a [code_formatter]() that takes into account
/// `\synopsis` overrides and other flags in the documentation that affect
/// formatting.
class synopsis_formatter {
 public:
  struct synopsis_formatter_options : inja_formatter::inja_formatter_options {
    synopsis_formatter_options();
  };

  explicit synopsis_formatter(synopsis_formatter_options);

  model::document build(const model::cpp_entity_documentation&) const;

  model::document build(const model::group_documentation&) const;

 private:
  synopsis_formatter_options options;
};

}

#endif
