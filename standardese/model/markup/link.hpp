// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_MODEL_MARKUP_LINK_HPP_INCLUDED
#define STANDARDESE_MODEL_MARKUP_LINK_HPP_INCLUDED

#include <type_safe/variant.hpp>

#include "../mixin/visitable.hpp"
#include "../mixin/container.hpp"
#include "../link_target.hpp"

namespace standardese::model::markup
{
    /// A hyperlink to another part of the documentation, some external
    /// documentation, or an external URL.
    class link final: public mixin::visitable<link>, public mixin::container<>
    {
    public:
      template <typename ...Args>
      link(link_target target, std::string title, Args&&... args) : target(target), title(title), mixin::container<>(std::forward<Args>(args)...) {
      }

      /// The title which will usually be shown as a tooltip. Note that links
      /// specified as `[link](<> "target")` will not have a title but instead
      /// the title will be used as the target here.
      std::string title;

      /// The target of this link exactly as specified in the input.
      link_target target;
    };
}

#endif
