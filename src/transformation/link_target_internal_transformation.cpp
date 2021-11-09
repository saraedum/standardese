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
#include "../../standardese/model/visitor/recursive_visitor.hpp"
#include "../../standardese/model/visitor/generic_visitor.hpp"
#include "../../standardese/model/unordered_entities.hpp"
#include "../../standardese/inventory/symbols.hpp"
#include "../../standardese/logger.hpp"

// TODO: Move all the implementations to the bottom and have documented types
// at the top.

namespace standardese::transformation
{

namespace {

// Creates an inventory of all the C++ entities that are documented in all the documents.
struct entities : model::visitor::generic_visitor<entities, model::visitor::recursive_visitor<true>> {
  template <typename T>
  void operator()(const T& entity)
  {
    if constexpr (std::is_base_of_v<model::cpp_entity_documentation, T>)
      entities.push_back(&entity.entity());

    model::visitor::recursive_visitor<true>::visit(entity);
  }

  std::vector<const cppast::cpp_entity*> entities;
};

// Creates an inventory of all the C++ headers files that are explicitly documented.
struct headers : model::visitor::generic_visitor<headers, model::visitor::recursive_visitor<true>> {
  template <typename T>
  void operator()(const T& entity) {
    if constexpr (std::is_base_of_v<model::cpp_entity_documentation, T>) {
      if (entity.entity().kind() == cppast::cpp_file::kind())
        headers.push_back(static_cast<const cppast::cpp_file*>(&entity.entity()));
    }

    model::visitor::recursive_visitor<true>::visit(entity);
  }

  std::vector<const cppast::cpp_file*> headers;
};

struct visitor : model::visitor::recursive_visitor<false> {
  visitor(const inventory::cppast_inventory& inventory, const inventory::files& files) : symbols(inventory), files(files) {}

  void visit(link& link) override {
    link.target.accept([&](auto&& target) {
      using T = std::decay_t<decltype(target)>;
      if constexpr (std::is_same_v<T, model::link_target::standardese_target>) {
        if (target.target == "") return;

        {
          // TODO: This is a hack, see heading transformation.
          const static std::regex entity_pattern{"^standardese://@([0-9]*)$"};
          std::smatch match;
          if (std::regex_match(target.target, match, entity_pattern)) {
            link.target = *static_cast<const cppast::cpp_entity*>((void*)atol(match[1].str().c_str()));
            return;
          }
        }

        // TODO: Perform relative lookup.
        {
          const auto entity = files.find_header(target.target);
          if (entity) {
            link.target = std::move(entity.value());
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
        // TODO: Perform relative lookup.
        {
          auto entity = symbols.find(target.target);
          if (entity.has_value()) {
            link.target = std::move(entity.value());
            return;
          }
        }
      }
    });

    model::visitor::recursive_visitor<false>::visit(link);
  }

 private:
  const inventory::symbols symbols;
  const inventory::files& files;
};

}

link_target_internal_transformation::link_target_internal_transformation(model::unordered_entities& documents, const parser::cpp_context& context) :
  transformation(documents),
  symbols([&]() {
    struct entities entities{};
    for (const auto& document : documents)
      document.accept(entities);
    return entities.entities;
  }(), context),
  files([&]() {
    struct headers headers{};
    for (const auto& document : documents)
      document.accept(headers);
    return headers.headers;
  }()) {
}

void link_target_internal_transformation::do_transform(model::entity& document) {
  visitor visitor{symbols, files};
  document.accept(visitor);
}

}
