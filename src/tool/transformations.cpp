// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <stdexcept>

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
#include "../../standardese/transformation/link_href_sphinx_transformation.hpp"

namespace standardese::tool {

namespace {

void transform_external_target(model::unordered_entities& documents, const struct transformations::options::external_link_options& external);

void transform_external_href(model::unordered_entities& documents, const struct transformations::options::external_link_options& external);

}

transformations::transformations(struct options options) : options(options) {}

void transformations::transform(model::unordered_entities& documents, const parser::cpp_context& context) {
  // TODO: Make this configurable

  // TODO: Use parallel worker pool.

  transformation::synopsis_transformation{documents}.transform();
  transformation::entity_heading_transformation{documents}.transform();;
  transformation::exclude_uncommented_transformation{documents}.transform();
  transformation::output_group_transformation{documents}.transform();
  transformation::anchor_transformation{documents}.transform();

  // Resolve Links in Standardese Syntax to their Targets
  transformation::link_target_internal_transformation{documents, context}.transform();
  for (auto& external : options.external_link_options)
    transform_external_target(documents, external);
  transformation::link_target_unresolved_transformation{documents}.transform();

  // Resolve Links to the actual URLs
  for (auto& external : options.external_link_options)
    transform_external_href(documents, external);
  transformation::link_href_internal_transformation{documents}.transform();
}

namespace {

void transform_external_target(model::unordered_entities& documents, const struct transformations::options::external_link_options& external) {
  switch(external.kind) {
    case transformations::options::external_link_options::kind::SPHINX:
      {
        // TODO: This is quite clumsy. Can we simplify the ownership questions
        // here? Maybe the options should already contain the inventory? And
        // the transformation take the inventory and not the options?
        const auto inventory = inventory::sphinx::documentation_set::parse(external.inventory.native());
        const auto symbols = inventory::symbols(inventory);
        transformation::link_target_external_transformation{documents, &symbols}.transform();
        break;
      }
    case transformations::options::external_link_options::kind::DOXYGEN:
      // TODO: Implement me.
      [[fallthrough]];
    default:
      throw std::logic_error("not implemented: cannot link to this kind of external documentation yet");
  }
}

void transform_external_href(model::unordered_entities& documents, const struct transformations::options::external_link_options& external) {
  switch(external.kind) {
    case transformations::options::external_link_options::kind::SPHINX:
      {
        // TODO: Do we have to parse again here? We do not need the actual inventory.
        const auto inventory = inventory::sphinx::documentation_set::parse(external.inventory.native());
        transformation::link_href_sphinx_transformation{documents, inventory.project, inventory.version, external.url}.transform();
        break;
      }
    case transformations::options::external_link_options::kind::DOXYGEN:
      // TODO: Implement me.
      [[fallthrough]];
    default:
      throw std::logic_error("not implemented: cannot link to this kind of external documentation yet");
  }
}

}

}
