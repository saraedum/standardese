// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <stdexcept>
#include <variant>

#include "../../standardese/model/entity.hpp"
#include "../../standardese/tool/transformers.hpp"
#include "../../standardese/model/unordered_entities.hpp"
#include "../../standardese/transformer/create_entity_heading_transformer.hpp"
#include "../../standardese/transformer/create_synopsis_transformer.hpp"
#include "../../standardese/transformer/exclude_uncommented_transformer.hpp"
#include "../../standardese/transformer/create_output_section_heading_transformer.hpp"
#include "../../standardese/transformer/set_id_transformer.hpp"
#include "../../standardese/transformer/set_href_internal_transformer.hpp"
#include "../../standardese/transformer/set_target_external_transformer.hpp"
#include "../../standardese/transformer/warn_target_unresolved_transformer.hpp"
#include "../../standardese/transformer/set_target_internal_transformer.hpp"
#include "../../standardese/transformer/set_href_sphinx_transformer.hpp"
#include "../../standardese/transformer/group_uncommented_transformer.hpp"
#include "../../standardese/transformer/merge_group_transformer.hpp"
#include "../../standardese/transformer/create_entity_document_transformer.hpp"
#include "../../standardese/transformer/create_index_document_transformer.hpp"
#include "../../standardese/transformer/create_uncommented_module_transformer.hpp"
#include "../../standardese/transformer/create_uncommented_child_transformer.hpp"

namespace standardese::tool {

template<class> inline constexpr bool always_false_v = false;

transformers::transformers(transformer_options options) : options(options) {}

void transformers::transform(model::unordered_entities& entities, const parser::cpp_context& context) {
  // TODO(0.6.0-alpha): Make this configurable

  // TODO(0.6.0-alpha): Use parallel worker pool.

  for (auto& child: transformer::create_uncommented_child_transformer{&entities, context}.transform())
    entities.insert(std::move(child));

  // Create (empty) documentation for modules that are referenced in the
  // documentation but have no documentation on their own.
  for (auto& module: transformer::create_uncommented_module_transformer{&entities}.transform())
    entities.insert(std::move(module));

  // Create documents for the entities that we want to document explicitly,
  // such as tho header files.
  auto documents = transformer::create_entity_document_transformer{&entities, context, options.entity_document_options}.transform();

  // TODO(0.6.0-alpha):
  // Create index documents listing all entities of a certain kind.
  // transformer::create_index_document_transformer{entities, context, options.index_document_options}.transform();

  // TODO(0.6.0-alpha): Make sure document names/paths are unique.

  // Resolve Links in Standardese Syntax to Internal Targets
  transformer::set_target_internal_transformer{&entities, context}.transform();

  // Resolve Links in Standardese Syntax to External Targets
  for (auto& option : options.external_link_options)
    std::visit([&](const auto& external) {
      using T = std::decay_t<decltype(external)>;
      if constexpr (std::is_same_v<T, transformer_options::external_sphinx_options>) {
        transformer::set_href_sphinx_transformer{&entities, external.options, inventory::sphinx::documentation_set::parse(external.inventory.native())}.transform();
      } else if constexpr (std::is_same_v<T, transformer_options::external_doxygen_options>) {
        // TODO(0.6.0-alpha): implement me.
        throw std::logic_error("not implemented: doxygen linking");
      } else if constexpr (std::is_same_v<T, transformer_options::external_legacy_options>) {
        transformer::set_href_external_legacy_transformer{&entities, external.options}.transform();
        throw std::logic_error("not implemented: legacy linking");
      } else {
        static_assert(always_false_v<T>, "unhandled external documentation link type");
      }
    }, option);
  transformer::warn_target_unresolved_transformer{&entities}.transform();

  transformer::group_uncommented_transformer{&entities, options.group_uncommented_options}.transform();

  transformer::merge_group_transformer{&entities, options.group_options}.transform();

  // TODO(0.6.0-alpha): Change the implementation of exclusion:
  // * the transformers here should just set the exclude_mode of entities
  // * then a final transformer should kick entities out, as currently the exclude_uncommented_transformer does.
  transformer::exclude_uncommented_transformer{&entities, options.exclude_uncommented_options}.transform();
  // TODO(0.6.0-alpha): Implement me.
  // transformer::exclude_access_transformer{...}.transform();
  // TODO(0.6.0-alpha): Implement me.
  // transformer::exclude_pattern_transformer{...}.transform();

  transformer::create_synopsis_transformer{&entities}.transform();
  transformer::create_entity_heading_transformer{&entities, context, options.entity_heading_options}.transform();;
  transformer::create_output_section_heading_transformer{&entities}.transform();
  transformer::set_id_transformer{&entities}.transform();

  // Resolve Links to the actual URLs
  transformer::set_href_internal_transformer{&entities, context}.transform();
}

}
