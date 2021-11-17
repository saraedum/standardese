// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//               2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_TOOL_TRANSFORMERS_HPP_INCLUDED
#define STANDARDESE_TOOL_TRANSFORMERS_HPP_INCLUDED

#include <boost/filesystem.hpp>

#include "../transformer/exclude_pattern_transformer.hpp"
#include "../transformer/exclude_access_transformer.hpp"
#include "../transformer/exclude_uncommented_transformer.hpp"
#include "../transformer/create_synopsis_transformer.hpp"
#include "../transformer/set_target_external_transformer.hpp"
#include "../transformer/set_href_external_legacy_transformer.hpp"
#include "../transformer/set_href_sphinx_transformer.hpp"
#include "../transformer/group_uncommented_transformer.hpp"
#include "../transformer/merge_group_transformer.hpp"
#include "../transformer/create_entity_heading_transformer.hpp"
#include "../transformer/create_entity_document_transformer.hpp"
#include "../transformer/create_index_document_transformer.hpp"

namespace standardese::tool {

/// Apply transformers to the output documents to give their outline the
/// final structure that can be emitted by the output generators.
/// This wraps all the transformers that the standardese executable normally
/// performs.
class transformers {
 public:
  struct transformer_options {
    transformer::group_uncommented_transformer::group_uncommented_options group_uncommented_options;
    transformer::merge_group_transformer::group_options group_options;
    transformer::exclude_pattern_transformer::exclude_pattern_transformer_options exclude_pattern_options;
    transformer::exclude_access_transformer::exclude_access_transformer_options exclude_access_options;
    transformer::exclude_uncommented_transformer::exclude_uncommented_transformer_options exclude_uncommented_options;
    transformer::create_synopsis_transformer::create_synopsis_transformer_options synopsis_options;
    transformer::create_entity_heading_transformer::create_entity_heading_transformer_options entity_heading_options;
    transformer::create_entity_document_transformer::create_entity_document_transformer_options entity_document_options;
    transformer::create_index_document_transformer::create_index_document_transformer_options index_document_options;

    struct external_sphinx_options {
      /// The local path of the inventory file.
      boost::filesystem::path inventory;

      transformer::set_href_sphinx_transformer::set_href_sphinx_transformer_options options;
    };

    struct external_doxygen_options {
      /// The local path of the inventory file.
      boost::filesystem::path inventory;

      transformer::set_href_sphinx_transformer::set_href_sphinx_transformer_options options;
    };

    struct external_legacy_options {
      transformer::set_href_external_legacy_transformer::set_href_external_legacy_transformer_options options;
    };

    using external_link_option = std::variant<external_sphinx_options, external_doxygen_options, external_legacy_options>;

    /// How to establish links to external documentation.
    std::vector<external_link_option> external_link_options;
  };

  transformers(transformer_options);

  /// Apply the configured transformers.
  void transform(model::unordered_entities& documents, const parser::cpp_context& context);

 private:
  transformer_options options;
};

}

#endif


