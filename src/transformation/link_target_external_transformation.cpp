// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <fmt/format.h>
#include <regex>
#include <cstdlib>

#include <cppast/cpp_file.hpp>

#include "../../standardese/transformation/link_target_external_transformation.hpp"
#include "../../standardese/model/visitor/visit.hpp"
#include "../../standardese/model/markup/link.hpp"
#include "../../standardese/model/entity.hpp"

namespace standardese::transformation
{

link_target_external_transformation::link_target_external_transformation(model::unordered_entities& documents, inventory::symbols symbols) :
  transformation(documents),
  symbols(std::move(symbols)) {}

void link_target_external_transformation::do_transform(model::entity& document) {
  model::visitor::visit([&](auto&& link) {
    using T = std::decay_t<decltype(link)>;
    if constexpr (std::is_same_v<T, model::markup::link>) {
      link.target.accept([&](auto&& target) {
        using T = std::decay_t<decltype(target)>;
        if constexpr (std::is_same_v<T, model::link_target::standardese_target>) {
          // TODO: Support relative lookup here.
          auto search = symbols.find(target.target);
          if (search)
            link.target = search.value();
        }

        // TODO: Handle the special schema:// here.
      });
    }

    return model::visitor::recursion::RECURSE;
  }, document);
}

}
