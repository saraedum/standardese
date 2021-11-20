// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <fmt/format.h>
#include <cstdlib>

#include <cppast/cpp_file.hpp>

#include "../../standardese/transformer/set_target_external_transformer.hpp"
#include "../../standardese/model/visitor/visit.hpp"
#include "../../standardese/model/markup/link.hpp"
#include "../../standardese/model/entity.hpp"
#include "../util/regex.hpp"

namespace standardese::transformer
{

set_target_external_transformer::set_target_external_transformer_options::set_target_external_transformer_options() {}

set_target_external_transformer::set_target_external_transformer(model::entity_set* documents, inventory::symbols symbols, set_target_external_transformer_options options) :
  inner_transformer(documents),
  symbols(std::move(symbols)),
  options(std::move(options)) {}

void set_target_external_transformer::do_transform(model::entity& document) {
  model::visitor::visit([&](auto&& link, auto&& recurse) {
    using T = std::decay_t<decltype(link)>;
    if constexpr (std::is_same_v<T, model::markup::link>) {
      link.target.accept([&](auto&& target) {
        using T = std::decay_t<decltype(target)>;
        if constexpr (std::is_same_v<T, model::link_target::standardese_target>) {

          const auto resolve = [&](const std::string& target) {
            auto search = symbols.find(target);
            if (search) {
              link.target = search.value();
              return true;
            }
            return false;
          };

          if (!options.schema.empty()) {
            std::smatch match;
            if (std::regex_search(target.target, match, util::regex::set_target_external_transformer_schema_pattern)) {
              const auto& schema = match[1];
              if (schema == options.schema)
                if (resolve(target.target.substr(match.length())))
                  return;
            }
          }

          if (!options.require_schema) {
            // TODO(0.6.0-beta): Support relative lookup here.
            if (resolve(target.target))
              return;
          }
        }
      });
    }

    recurse();
  }, document);
}

}
