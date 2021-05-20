// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <fmt/format.h>
#include <regex>
#include <cstdlib>

#include <cppast/cpp_file.hpp>

#include "../../standardese/transformation/link_target_internal_transformation.hpp"
#include "../../standardese/model/visitor/visit.hpp"
#include "../../standardese/model/markup/link.hpp"
#include "../../standardese/model/cpp_entity_documentation.hpp"
#include "../../standardese/model/unordered_entities.hpp"
#include "../../standardese/inventory/symbols.hpp"
#include "../../standardese/logger.hpp"

namespace standardese::transformation {

link_target_internal_transformation::link_target_internal_transformation(model::unordered_entities& documents, const parser::cpp_context& context) :
  transformation(documents),
  inventory([&]() {
    // Create an inventory of all the C++ entities that are documented in all the documents.
    std::vector<const cppast::cpp_entity*> entities;

    for (const auto& document : documents) {
      model::visitor::visit([&](auto&& entity, auto&& recurse) {
        using T = std::decay_t<decltype(entity)>;
        if constexpr (std::is_base_of_v<model::cpp_entity_documentation, T>)
          entities.push_back(&entity.entity());
        recurse();
      }, document);
    }

    return entities;
  }(), context),
  files([&]() {
    // Create an inventory of all the C++ headers files that are explicitly documented.
    std::vector<const cppast::cpp_file*> headers;

    for (const auto& document : documents) {
      model::visitor::visit([&](auto&& entity, auto&& recurse) {
        using T = std::decay_t<decltype(entity)>;
        if constexpr (std::is_base_of_v<model::cpp_entity_documentation, T>) {
          if (entity.entity().kind() == cppast::cpp_file::kind())
            headers.push_back(static_cast<const cppast::cpp_file*>(&entity.entity()));
        }
        recurse();
      }, document);
    }

    return headers;
  }()) {
}

void link_target_internal_transformation::do_transform(model::entity& document) {
  inventory::symbols symbols{inventory};
  std::stack<type_safe::object_ref<const cppast::cpp_entity>> relative;

  model::visitor::visit([&](auto& link, auto&& recurse) {
    using T = std::decay_t<decltype(link)>;
    if constexpr (std::is_same_v<T, model::cpp_entity_documentation>) {
      relative.push(type_safe::ref(link.entity()));
      recurse();
      relative.pop();
      return;
    }

    if constexpr (std::is_same_v<T, model::markup::link>) {
      link.target.accept([&](auto&& target) {
        using T = std::decay_t<decltype(target)>;
        if constexpr (std::is_same_v<T, model::link_target::standardese_target>) {
          if (target.target == "") return;

          {
            // TODO: This is a hack, see heading transformation.
            const static std::regex entity_pattern{"^standardese://@([0-9]*)$"};
            std::smatch match;
            if (std::regex_match(target.target, match, entity_pattern)) {
              link.target = model::link_target(*static_cast<const cppast::cpp_entity*>((void*)atol(match[1].str().c_str())));
              return;
            }
          }

          // TODO: Perform relative lookup.
          {
            const auto entity = files.find_header(target.target);
            if (entity) {
              link.target = model::link_target(std::move(entity.value()));
              return;
            }
          }

          // Taken from RFC3986 p.50. Adapted so that scheme & authority are not optional.
          const static std::regex uri_pattern{R"(^(([^:/?#]+):)(//([^/?#]*))([^?#]*)(\?([^#]*))?(#(.*))?)"};
          std::smatch match;
          if (std::regex_match(target.target, match, uri_pattern)) {
            // TODO: Handle standardese:// schemes here.
            return;
          }

          // TODO: Handle \unique_name
          {
            auto entity = relative.size() ?
              symbols.find(target.target, *relative.top()) :
              symbols.find(target.target);

            if (entity.has_value())
              link.target = std::move(entity.value());
          }
        }
      });
    }

    recurse();
  }, document);
}

}
