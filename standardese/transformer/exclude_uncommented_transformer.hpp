// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_TRANSFORMER_EXCLUDE_UNCOMMENTED_TRANSFORMER_HPP_INCLUDED
#define STANDARDESE_TRANSFORMER_EXCLUDE_UNCOMMENTED_TRANSFORMER_HPP_INCLUDED

#include "inner_transformer.hpp"

namespace standardese::transformer {

// TODO(0.6.0-beta): Add options to only exclude uncommented entities if they are not linked to.

/// Exclude uncommented entities from the output.
class exclude_uncommented_transformer : public inner_transformer {
  public:
    struct exclude_uncommented_transformer_options {
      enum class mode {
        /// Exclude this entity if uncommented.
        exclude,
        /// Exclude this entity if uncommented and all its members are uncommented.
        exclude_if_empty,
        /// Exclude this entity if uncommented but include the children of this entity.
        skip,
        /// Include this entity even if uncommented.
        include,
      };

      /// Whether to exclude header files without a comment on the file level.
      mode exclude_file = mode::include;

      /// Whether to exclude namespaces without a comment on the namespace.
      mode exclude_namespace = mode::skip;

      /// Whether to exclude uncommented structs and classes.
      mode exclude_class = mode::include;

      /// Whether to exclude uncommented functions.
      mode exclude_function = mode::include;

      /// Whether to exclude uncommented aliases such as `using` or `typedef`.
      mode exclude_alias = mode::include;

      /// Whether to exclude uncommented variables.
      mode exclude_variable = mode::include;

      /// Whether to exclude uncommented friends.
      mode exclude_friend = mode::include;

      /// Whether to exclude uncommented macro definitions.
      mode exclude_macro = mode::include;
    };

    explicit exclude_uncommented_transformer(model::entity_set* documents, exclude_uncommented_transformer_options);

  protected:
    void do_transform(model::entity& root) override;

  private:
    exclude_uncommented_transformer_options options;
};

}

#endif

