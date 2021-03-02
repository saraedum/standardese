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

namespace standardese::tool {

/// Apply transformations to the output documents to give their outline the
/// final structure that can be emitted by the output generators.
/// This wraps all the transformations that the standardese executable normally
/// performs.
class transformations {
 public:
  struct options {
    struct transformation::exclude_pattern_transformation::options exclude_pattern_options;
    struct transformation::exclude_access_transformation::options exclude_access_options;
    struct transformation::exclude_uncommented_transformation::options exclude_uncommented_options;
    struct transformation::synopsis_transformation::options synopsis_options;

    /// How to establish links to external documentation.
    struct external_link_options {
      /// A URI schema to explicitly link to some external documentation such
      /// as `std` to use `std://` to link to the standard library.
      std::string schema;

      /// The base URL of the external documentation.
      /// Inventories usually give all paths relative to this base URL, e.g.,
      /// `https://en.cppreference.com/w/` for the standard library.
      std::string url;
      
      /// The local path of the inventory file.
      boost::filesystem::path inventory;

      struct transformation::link_target_external_transformation::options link_target_options;

      enum class kind {
        /// `inventory` is a doxygen tag file.
        DOXYGEN,
        /// `inventory` is a spinx inventory file.
        SPHINX,
      } kind;
    };

    std::vector<struct external_link_options> external_link_options;
  };

  transformations(options);

  /// Apply the configured transformations.
  void transform(model::unordered_entities& documents, const parser::cpp_context& context);

 private:
  struct options options;
};

}

#endif


