// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <stdexcept>
#include <variant>

#include "../../standardese/tool/transformations.hpp"
#include "../../standardese/model/unordered_entities.hpp"
#include "../../standardese/transformation/entity_heading_transformation.hpp"
#include "../../standardese/transformation/synopsis_transformation.hpp"
#include "../../standardese/transformation/exclude_uncommented_transformation.hpp"
#include "../../standardese/transformation/output_group_transformation.hpp"
#include "../../standardese/transformation/anchor_transformation.hpp"
#include "../../standardese/transformation/link_target_internal_transformation.hpp"
#include "../../standardese/transformation/link_target_external_transformation.hpp"
#include "../../standardese/transformation/link_target_unresolved_transformation.hpp"
#include "../../standardese/transformation/link_href_internal_transformation.hpp"
#include "../../standardese/transformation/link_sphinx_transformation.hpp"
#include "../../standardese/transformation/group_uncommented_transformation.hpp"
#include "../../standardese/transformation/group_transformation.hpp"

namespace standardese::tool {

template<class> inline constexpr bool always_false_v = false;

transformations::transformations(struct options options) : options(options) {}

void transformations::transform(model::unordered_entities& documents, const parser::cpp_context& context) {
  // TODO: Make this configurable

  // TODO: Use parallel worker pool.

  // Resolve Links in Standardese Syntax to Internal Targets
  transformation::link_target_internal_transformation{documents, context}.transform();

  // Resolve Links in Standardese Syntax to External Targets
  for (auto& option : options.external_link_options)
    std::visit([&](const auto& external) {
      using T = std::decay_t<decltype(external)>;
      if constexpr (std::is_same_v<T, options::external_sphinx_options>) {
        transformation::link_sphinx_transformation{documents, external.options, inventory::sphinx::documentation_set::parse(external.inventory.native())}.transform();
      } else if constexpr (std::is_same_v<T, options::external_doxygen_options>) {
        // TODO: implement me.
        throw std::logic_error("not implemented: doxygen linking");
      } else if constexpr (std::is_same_v<T, options::external_legacy_options>) {
        transformation::link_external_legacy_transformation{documents, external.options}.transform();
        throw std::logic_error("not implemented: legacy linking");
      } else {
        static_assert(always_false_v<T>, "unhandled external documentation link type");
      }
    }, option);
  transformation::link_target_unresolved_transformation{documents}.transform();

  transformation::group_uncommented_transformation{documents, options.group_uncommented_options}.transform();

  transformation::group_transformation{documents, options.group_options}.transform();

  transformation::exclude_uncommented_transformation{documents, options.exclude_uncommented_options}.transform();
  transformation::synopsis_transformation{documents}.transform();
  transformation::entity_heading_transformation{documents, context, options.entity_heading_options}.transform();;
  transformation::output_group_transformation{documents}.transform();
  transformation::anchor_transformation{documents}.transform();

  // Resolve Links to the actual URLs
  transformation::link_href_internal_transformation{documents, context}.transform();
}

}
