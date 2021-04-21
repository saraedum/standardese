// Copyright (C) 2017 Jonathan Müller <jonathanmueller.dev@gmail.com>
//               2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_FORMATTER_INJA_FORMATTER_HPP_INCLUDED
#define STANDARDESE_FORMATTER_INJA_FORMATTER_HPP_INCLUDED

#include <string>

#include <cppast/forward.hpp>

#include "../forward.hpp"

namespace standardese::formatter {

/// Renders C++ entities using the Inja templating engine.
class inja_formatter {
 public:
  struct options {
    explicit options();
    explicit options(const std::string& format);

    std::string file_format = "{{ filename(path) }}";
    std::string function_format = "{{ name }}";
    std::string type_alias_format = "{{ name }}";
    std::string class_format = "{{ name }}";
    std::string member_variable_format = "{{ name }}";
  };

  explicit inja_formatter(options);

  /// Format this entity using its configured inja template.
  /// The entity is rendered with inja and then parsed again as MarkDown.
  /// Returns an entity containing the root node of the generated markup.
  model::entity format(const cppast::cpp_entity&) const;

  /// Format this entity using its configured inja template.
  /// The entity is rendered with inja and then parsed again as MarkDown.
  /// Returns an entity containing the root node of the generated markup.
  model::entity format(const model::module&) const;

 private:
  options options;
};

}

#endif
