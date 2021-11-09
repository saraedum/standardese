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
#include "../transformer/synopsis_transformer.hpp"
#include "../transformer/link_target_external_transformer.hpp"
#include "../transformer/link_external_legacy_transformer.hpp"
#include "../transformer/link_sphinx_transformer.hpp"
#include "../transformer/group_uncommented_transformer.hpp"
#include "../transformer/group_transformer.hpp"
#include "../transformer/entity_heading_transformer.hpp"

namespace standardese::tool {

/// Apply transformers to the output documents to give their outline the
/// final structure that can be emitted by the output generators.
/// This wraps all the transformers that the standardese executable normally
/// performs.
class transformers {
 public:
  struct options {
    struct transformer::group_uncommented_transformer::group_uncommented_options group_uncommented_options;
    struct transformer::group_transformer::group_options group_options;
    struct transformer::exclude_pattern_transformer::options exclude_pattern_options;
    struct transformer::exclude_access_transformer::options exclude_access_options;
    struct transformer::exclude_uncommented_transformer::options exclude_uncommented_options;
    struct transformer::synopsis_transformer::options synopsis_options;
    struct transformer::entity_heading_transformer::entity_heading_transformer_options entity_heading_options;

    struct external_sphinx_options {
      /// The local path of the inventory file.
      boost::filesystem::path inventory;

      struct transformer::link_sphinx_transformer::options options;
    };

    struct external_doxygen_options {
      /// The local path of the inventory file.
      boost::filesystem::path inventory;

      struct transformer::link_sphinx_transformer::options options;
    };

    struct external_legacy_options {
      struct transformer::link_external_legacy_transformer::options options;
    };

    using external_link_option = std::variant<external_sphinx_options, external_doxygen_options, external_legacy_options>;

    /// How to establish links to external documentation.
    std::vector<external_link_option> external_link_options;
  };

  transformers(options);

  /// Apply the configured transformers.
  void transform(model::unordered_entities& documents, const parser::cpp_context& context);

 private:
  struct options options;
};

}

#endif


