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
#include "../../standardese/model/visitor/recursive_visitor.hpp"
#include "../../standardese/model/visitor/generic_visitor.hpp"
#include "../../standardese/logger.hpp"

// TODO: We need an additional transformation that optionally turns [target]() into [`target`]().

// TODO: Move all the code to the bottom and declare and document types at the top.

namespace standardese::transformation
{

namespace {

// Collects all the entities that can be linked to in our documents.
struct anchors : model::visitor::generic_visitor<anchors, model::visitor::recursive_visitor<true>> {
  template <typename T>
  void operator()(const T& entity)
  {
    if constexpr (std::is_base_of_v<model::document, T>) {
      path = entity.path;
    }
    else if constexpr (std::is_base_of_v<model::cpp_entity_documentation, T>) {
      a[&entity.entity()] = path + "/#" + entity.id;
    }
    model::visitor::recursive_visitor<true>::visit(entity);
  }

  std::string path;
  std::unordered_map<const cppast::cpp_entity*, std::string> a;
};

}

link_href_internal_transformation::link_href_internal_transformation(model::unordered_entities& documents) :
  transformation(documents),
  anchors([&]() {
    struct anchors anchors{};
    for (const auto& document : documents)
      document.accept(anchors);
    return anchors.a;
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
