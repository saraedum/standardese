// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <cmark-gfm.h>

#include "../../../standardese/output_generator/markdown/markdown_generator.hpp"

namespace standardese::output_generator::markdown
{

template <typename T, auto free>
using unique_cmark = std::unique_ptr<T, std::integral_constant<decltype(free), free>>;

markdown_generator::markdown_generator_options::markdown_generator_options() {}

markdown_generator::markdown_generator(std::ostream& os, markdown_generator_options options) : cmark_generator(os), options(std::move(options)) {}

markdown_generator::~markdown_generator() {
  using unique_string = unique_cmark<char, free>;
  unique_string str{cmark_render_commonmark(root.get(), CMARK_OPT_NOBREAKS, 0)};
  out_ << str.get();
}

}
