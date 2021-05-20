// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <cmark-gfm.h>

#include "../../../standardese/output_generator/text/text_generator.hpp"

#include "../../../standardese/model/markup/text.hpp"

namespace standardese::output_generator::text {

template <typename T, auto free>
using unique_cmark = std::unique_ptr<T, std::integral_constant<decltype(free), free>>;

text_generator::text_generator_options::text_generator_options() {}

text_generator::text_generator(std::ostream& os, text_generator_options options) : cmark_generator(os) {}

text_generator::~text_generator() {
  using unique_string = unique_cmark<char, free>;
  unique_string str{cmark_render_plaintext(root.get(), CMARK_OPT_NOBREAKS, 0)};
  out_ << str.get();
}

std::string text_generator::render(const model::entity& root) {
  return stream_generator::render<text_generator>(root);
}

}
