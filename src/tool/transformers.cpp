// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <stdexcept>
#include <variant>

#include "../../standardese/tool/transformers.hpp"
#include "../../standardese/model/unordered_entities.hpp"
#include "../../standardese/transformer/entity_heading_transformer.hpp"
#include "../../standardese/transformer/synopsis_transformer.hpp"
#include "../../standardese/transformer/exclude_uncommented_transformer.hpp"
#include "../../standardese/transformer/output_group_transformer.hpp"
#include "../../standardese/transformer/anchor_transformer.hpp"
#include "../../standardese/transformer/link_target_internal_transformer.hpp"
#include "../../standardese/transformer/link_target_external_transformer.hpp"
#include "../../standardese/transformer/link_target_unresolved_transformer.hpp"
#include "../../standardese/transformer/link_href_internal_transformer.hpp"
#include "../../standardese/transformer/link_sphinx_transformer.hpp"
#include "../../standardese/transformer/group_uncommented_transformer.hpp"
#include "../../standardese/transformer/group_transformer.hpp"

namespace standardese::tool {

template<class> inline constexpr bool always_false_v = false;

transformers::transformers(struct options options) : options(options) {}

void transformers::transform(model::unordered_entities& documents, const parser::cpp_context& context) {
  // TODO(0.6.0-alpha): Make this configurable

  // TODO(0.6.0-alpha): Use parallel worker pool.

  // Resolve Links in Standardese Syntax to Internal Targets
  transformer::link_target_internal_transformer{documents, context}.transform();

  // Resolve Links in Standardese Syntax to External Targets
  for (auto& option : options.external_link_options)
    std::visit([&](const auto& external) {
      using T = std::decay_t<decltype(external)>;
      if constexpr (std::is_same_v<T, options::external_sphinx_options>) {
        transformer::link_sphinx_transformer{documents, external.options, inventory::sphinx::documentation_set::parse(external.inventory.native())}.transform();
      } else if constexpr (std::is_same_v<T, options::external_doxygen_options>) {
        // TODO(0.6.0-alpha): implement me.
        throw std::logic_error("not implemented: doxygen linking");
      } else if constexpr (std::is_same_v<T, options::external_legacy_options>) {
        transformer::link_external_legacy_transformer{documents, external.options}.transform();
        throw std::logic_error("not implemented: legacy linking");
      } else {
        static_assert(always_false_v<T>, "unhandled external documentation link type");
      }
    }, option);
  transformer::link_target_unresolved_transformer{documents}.transform();

  transformer::group_uncommented_transformer{documents, options.group_uncommented_options}.transform();

  transformer::group_transformer{documents, options.group_options}.transform();

  transformer::exclude_uncommented_transformer{documents, options.exclude_uncommented_options}.transform();
  transformer::synopsis_transformer{documents}.transform();
  transformer::entity_heading_transformer{documents, context, options.entity_heading_options}.transform();;
  transformer::output_group_transformer{documents}.transform();
  transformer::anchor_transformer{documents}.transform();

  // Resolve Links to the actual URLs
  transformer::link_href_internal_transformer{documents, context}.transform();
}

}
