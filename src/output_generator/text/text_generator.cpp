// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <cmark-gfm.h>
#include <ostream>
#include <sstream>

#include "../../../standardese/output_generator/text/text_generator.hpp"
#include "../../../standardese/model/markup/link.hpp"
#include "../../../standardese/model/markup/text.hpp"

namespace standardese::output_generator::text {

template <typename T, auto free>
using unique_cmark = std::unique_ptr<T, std::integral_constant<decltype(free), free>>;

text_generator::text_generator_options::text_generator_options() {}

text_generator::text_generator(std::ostream* os, text_generator_options options) : cmark_generator(os) {}

text_generator::~text_generator() {
  using unique_string = unique_cmark<char, free>;
  unique_string str{cmark_render_plaintext(root.get(), CMARK_OPT_NOBREAKS, 0)};
  *out << str.get();
}

void text_generator::visit(link& link) {
  top = append_child(*top, CMARK_NODE_LINK);

  if (!link.title.empty())
    cmark_node_set_title(top, link.title.c_str());

  stream_generator::visit(link);
  top = cmark_node_parent(top);
}

std::string text_generator::render(const model::entity& root) {
    std::stringstream s;
    {
      auto generator = text_generator(&s);
      root.accept(generator);
    }
    return s.str();
}

}
