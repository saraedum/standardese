// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_OUTPUT_MARKDOWN_MARKDOWN_GENERATOR_HPP_INCLUDED
#define STANDARDESE_OUTPUT_MARKDOWN_MARKDOWN_GENERATOR_HPP_INCLUDED

#include <cmark-gfm.h>

#include "../cmark_generator.hpp"

namespace standardese::output_generator::markdown
{

class markdown_generator : public cmark_generator {
  public:
    struct markdown_generator_options {
      markdown_generator_options();

      enum class anchors {
        /// Do not emit anything not covered by the CommonMark standard. We
        /// rely on the MarkDown postprocessors such as jekyll to turn headings
        /// into something that can be linked to. (And produce `name`/`id`
        /// attributes that are compatible with the links we generated.)
        plain,
        /// Explicitly emit `<a>` tags next to all headings.
        html,
      };

      /// How to emit entities that can be linked to.
      anchors anchors = anchors::plain;
    };

    markdown_generator(std::ostream& os, markdown_generator_options = {});

    ~markdown_generator() override;

    static std::string render(const model::entity& root);

  private:
    markdown_generator_options options;
};

}

#endif

