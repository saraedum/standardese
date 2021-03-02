// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <fmt/format.h>
#include <regex>
#include <cstdlib>

#include <cppast/cpp_file.hpp>

#include "../../standardese/transformation/link_target_unresolved_transformation.hpp"
#include "../../standardese/model/visitor/visit.hpp"
#include "../../standardese/model/markup/link.hpp"
#include "../../standardese/model/visitor/recursive_visitor.hpp"
#include "../../standardese/model/visitor/generic_visitor.hpp"
#include "../../standardese/logger.hpp"

namespace standardese::transformation {

namespace {

/// Reports all standardese links that could not be resolved.
struct visitor : model::visitor::recursive_visitor<false> {
  void visit(link& link) override {
    link.target.accept([&](auto&& target) {
      using T = std::decay_t<decltype(target)>;
      if constexpr (std::is_same_v<T, model::link_target::standardese_target>) {
        if (target.target == "") {
          link.target = model::link_target::uri_target("");
          return;
        }

        standardese::logger::warn(fmt::format("Could not resolve link target `{}`.", target.target));
      }
    });

    model::visitor::recursive_visitor<false>::visit(link);
  }
};

}

link_target_unresolved_transformation::link_target_unresolved_transformation(model::unordered_entities& documents) : transformation(documents) {}

void link_target_unresolved_transformation::do_transform(model::entity& document) {
  visitor visitor{};
  document.accept(visitor);
}

}
