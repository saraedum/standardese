// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <fmt/format.h>
#include <regex>
#include <cstdlib>

#include "../../standardese/transformation/link_href_internal_transformation.hpp"
#include "../../standardese/formatter/inja_formatter.hpp"
#include "../../standardese/model/visitor/visit.hpp"
#include "../../standardese/model/markup/link.hpp"
#include "../../standardese/model/cpp_entity_documentation.hpp"
#include "../../standardese/model/group_documentation.hpp"
#include "../../standardese/output_generator/xml/xml_generator.hpp"
#include "../../standardese/formatter/inja_formatter.hpp"
#include "../../standardese/logger.hpp"

// TODO: We need an additional transformation that optionally turns [target]() into [`target`]().

namespace standardese::transformation
{

link_href_internal_transformation::link_href_internal_transformation(model::unordered_entities& documents, parser::cpp_context context) :
  transformation(documents),
  context(context),
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
  model::visitor::visit([&](auto&& entity, auto&& recurse) {
    using T = std::decay_t<decltype(entity)>;
    if constexpr (std::is_same_v<T, model::markup::link>) {
      entity.target.accept([&](auto&& target) -> void {
        using T = std::decay_t<decltype(target)>;
        if constexpr (std::is_same_v<T, model::link_target::module_target>) {
          throw std::logic_error("not implemented: resolve_module_target");
        } else if constexpr (std::is_same_v<T, model::link_target::cppast_target>) {
          // TODO: Use cppast ids instead?
          auto resolved = anchors.find(&*target.target);
          if (resolved == anchors.end()) {
              logger::error(fmt::format("Could not create URL for link to the {} `{}` from `{}`. Found the reference `{}`. Target was not found in inventory of C++ entities which are linkable.", formatter::inja_formatter{{}, context}.kind(*target.target), target.target->name(), formatter::inja_formatter{{}, context}.path(*target.target), output_generator::xml::xml_generator::render(document)));
              return;
          }

          // TODO: Make relative
          entity.target = model::link_target::uri_target("/" + resolved->second);
        }
      });
    }

    recurse();
  }, document);
}

}
