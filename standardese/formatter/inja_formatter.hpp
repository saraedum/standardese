// Copyright (C) 2017 Jonathan Müller <jonathanmueller.dev@gmail.com>
//               2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_FORMATTER_INJA_FORMATTER_HPP_INCLUDED
#define STANDARDESE_FORMATTER_INJA_FORMATTER_HPP_INCLUDED

#include <memory>
#include <string>

#include <cppast/forward.hpp>
#include <type_safe/optional_ref.hpp>

#include "../forward.hpp"

namespace standardese::formatter {

/// Renders C++ entities using the Inja templating engine.
class inja_formatter {
 public:
  struct options {
    options();

    // TODO: Do we need something like this?
    /*
    std::string file_format = "{{ filename(path) }}";
    std::string function_format = "{{ name }}";
    std::string type_alias_format = "{{ name }}";
    std::string class_format = "{{ name }}";
    std::string member_variable_format = "{{ name }}";
    */
  };

  class environment {
   public:
    environment();
    environment(const model::cpp_entity_documentation& context);
    environment(const model::module& context);
    ~environment();

   private:
    struct impl;
    friend class inja_formatter;

    std::unique_ptr<struct impl> self;
  };

  explicit inja_formatter(options);

  /// Render the inja template `format` using `context`.
  std::string format(const std::string& format, const environment& context) const;

  /// Render the inja template `format` using `context` and parse the result as MarkDown.
  /// Returns the root node of the generated markup tree.
  model::document build(const std::string& format, const environment& context) const;

 private:
  options options;
};

}

#endif
