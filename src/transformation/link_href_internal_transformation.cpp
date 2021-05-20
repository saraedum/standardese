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
#include "../../standardese/model/group_documentation.hpp"
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
      model::visitor::visit([&](auto&& entity, auto&& recurse) {
        using T = std::decay_t<decltype(entity)>;

        if constexpr (std::is_same_v<T, model::document>) {
          path = entity.path;
        } else if constexpr (std::is_same_v<T, model::cpp_entity_documentation>) {
          a[&entity.entity()] = path + "#" + entity.id;
        } else if constexpr (std::is_same_v<T, model::group_documentation>) {
          for (const auto& member : entity.entities) {
            a[&member.entity()] = path + "#" + entity.id;
          }
        }

        recurse();
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
              // TODO: Include kind in message.
              logger::error(fmt::format("Could not create URL for link to `{}`. Target was not found in inventory of C++ entities which are linkable.", target.target->name()));
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
