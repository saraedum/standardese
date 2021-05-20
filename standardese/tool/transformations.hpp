// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//               2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_TOOL_TRANSFORMATIONS_HPP_INCLUDED
#define STANDARDESE_TOOL_TRANSFORMATIONS_HPP_INCLUDED

#include <boost/filesystem.hpp>

#include "../transformation/exclude_pattern_transformation.hpp"
#include "../transformation/exclude_access_transformation.hpp"
#include "../transformation/exclude_uncommented_transformation.hpp"
#include "../transformation/synopsis_transformation.hpp"
#include "../transformation/link_target_external_transformation.hpp"
#include "../transformation/link_external_legacy_transformation.hpp"
#include "../transformation/link_sphinx_transformation.hpp"
#include "../transformation/group_uncommented_transformation.hpp"
#include "../transformation/group_transformation.hpp"
#include "../transformation/entity_heading_transformation.hpp"

namespace standardese::tool {

/// Apply transformations to the output documents to give their outline the
/// final structure that can be emitted by the output generators.
/// This wraps all the transformations that the standardese executable normally
/// performs.
class transformations {
 public:
  struct options {
    struct transformation::group_uncommented_transformation::group_uncommented_options group_uncommented_options;
    struct transformation::group_transformation::group_options group_options;
    struct transformation::exclude_pattern_transformation::options exclude_pattern_options;
    struct transformation::exclude_access_transformation::options exclude_access_options;
    struct transformation::exclude_uncommented_transformation::options exclude_uncommented_options;
    struct transformation::synopsis_transformation::options synopsis_options;
    struct transformation::entity_heading_transformation::entity_heading_transformation_options entity_heading_options;

    struct external_sphinx_options {
      /// The local path of the inventory file.
      boost::filesystem::path inventory;

      struct transformation::link_sphinx_transformation::options options;
    };

    struct external_doxygen_options {
      /// The local path of the inventory file.
      boost::filesystem::path inventory;

      struct transformation::link_sphinx_transformation::options options;
    };

    struct external_legacy_options {
      struct transformation::link_external_legacy_transformation::options options;
    };

    using external_link_option = std::variant<external_sphinx_options, external_doxygen_options, external_legacy_options>;

    /// How to establish links to external documentation.
    std::vector<external_link_option> external_link_options;
  };

  transformations(options);

  /// Apply the configured transformations.
  void transform(model::unordered_entities& documents, const parser::cpp_context& context);

 private:
  struct options options;
};

}

#endif


