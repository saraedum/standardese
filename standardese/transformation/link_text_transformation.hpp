// Copyright (C) 2017 Jonathan Müller <jonathanmueller.dev@gmail.com>
//               2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_TRANSFORMATION_ANCHOR_TEXT_TRANSFORMATION_HPP_INCLUDED
#define STANDARDESE_TRANSFORMATION_ANCHOR_TEXT_TRANSFORMATION_HPP_INCLUDED

#include "transformation.hpp"

namespace standardese::transformation {

/// Sets the displayed text for links that have an empty text.
/// This affects, e.g., links that have been created from the `[entity]()`
/// syntax.
class link_text_transformation : public transformation {
  public:
    struct link_text_transformation_options {
      link_text_transformation_options();

      /// The text to generate for links in standardese syntax `[entity]()`
      /// that have not been resolved yet.
      /// The default is to ignore them.
      std::string standardese_format = "";

      /// The text to generate for internal links to C/C++ entities.
      std::string cppast_format = "{{ `md_escape(name)` }}";

      /// The text to generate for internal links to modules.
      std::string module_format = "{{ md_escape(name) }}";

      /// The text to generate for links to external documentation generated
      /// with sphinx.
      /// The default is to use the "display name" as specified by the external
      /// documentation.
      std::string sphinx_format = "{{ `md_escape(display_name)` }}";

      /// The text to generated for links to external documentation generated
      /// with doxygen.
      /// The default is to use a style similar to the one we are using for
      /// internal documentation.
      std::string doxygen_format = "{{ `md_escape(name)` }}";

      /// The text to generate for links with an explicit URL.
      /// The default is not to generate any URL; if the user wrote
      /// `[](https://...)` we assume that they did this on purpose.
      std::string uri_format = "";
    };

    link_text_transformation(model::unordered_entities& documents, link_text_transformation_options options = {});

  protected:
    void do_transform(model::entity& root) override;

  private:
    link_text_transformation_options options;
};

}

#endif
