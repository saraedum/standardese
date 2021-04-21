// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <fmt/format.h>
#include <regex>
#include <cstdlib>

#include "../../standardese/transformation/link_href_internal_transformation.hpp"
#include "../../standardese/model/visitor/visit.hpp"
#include "../../standardese/model/markup/link.hpp"
#include "../../standardese/model/cpp_entity_documentation.hpp"
#include "../../standardese/logger.hpp"

// TODO: We need an additional transformation that optionally turns [target]() into [`target`]().

namespace standardese::transformation
{

link_href_internal_transformation::link_href_internal_transformation(model::unordered_entities& documents) :
  transformation(documents),
  anchors([&]() {
    std::string path;
    std::unordered_map<const cppast::cpp_entity*, std::string> a;

    for (const auto& document : documents)
      model::visitor::visit([&](auto&& entity) {
        using T = std::decay_t<decltype(entity)>;

        if constexpr (std::is_base_of_v<model::document, T>) {
          path = entity.path;
        } else if constexpr (std::is_base_of_v<model::cpp_entity_documentation, T>) {
          a[&entity.entity()] = path + "#" + entity.id;
        }

        return model::visitor::recursion::RECURSE;
      }, document);

    return a;
  }()) {
}

void link_href_internal_transformation::do_transform(model::entity& document) {
  struct visitor : model::visitor::recursive_visitor<false> {
    visitor(const std::unordered_map<const cppast::cpp_entity*, std::string>& inventory) : inventory(inventory) {}

    void visit(link& link) override {
      link.target.accept([&](auto&& target) -> void {
        using T = std::decay_t<decltype(target)>;
        if constexpr (std::is_same_v<T, model::link_target::module_target>) {
          throw std::logic_error("not implemented: resolve_module_target");
        } else if constexpr (std::is_same_v<T, model::link_target::cppast_target>) {
          // TODO: Use cppast ids instead?
          auto resolved = inventory.find(&*target.target);
          if (resolved == inventory.end()) {
              // TODO: Write a proper name for target.
              logger::error(fmt::format("Could not resolve link to `{}`.", target.target->name()));
              return;
          }

          // TODO: Make relative
          link.target = model::link_target::uri_target("/" + resolved->second);
        }
      });

      model::visitor::recursive_visitor<false>::visit(link);
    }

    const std::unordered_map<const cppast::cpp_entity*, std::string>& inventory;
  } visitor(anchors);

  document.accept(visitor);
}

}
