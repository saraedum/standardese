// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_MODEL_MARKUP_IMAGE_HPP_INCLUDED
#define STANDARDESE_MODEL_MARKUP_IMAGE_HPP_INCLUDED

#include <string>

#include "../mixin/visitable.hpp"
#include "../mixin/container.hpp"

namespace standardese::model::markup
{
    class image final: public mixin::container<>, public mixin::visitable<image> {
      public:
      image(std::string src, std::string title) : src(std::move(src)), title(std::move(title)) {}

      std::string src;
      std::string title;
    };
}

#endif

