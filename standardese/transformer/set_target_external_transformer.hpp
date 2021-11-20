// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_TRANSFORMER_SET_TARGET_EXTERNAL_TRANSFORMER_HPP_INCLUDED
#define STANDARDESE_TRANSFORMER_SET_TARGET_EXTERNAL_TRANSFORMER_HPP_INCLUDED

#include "inner_transformer.hpp"
#include "../inventory/symbols.hpp"

namespace standardese::transformer
{

/// Resolves links in MarkDown that use standardese syntax to link to an
/// external target such another standardese project, or documentation
/// generated with doxygen or sphinx.
class set_target_external_transformer : public inner_transformer {
  public:
    struct set_target_external_transformer_options {
      set_target_external_transformer_options();

      /// A URI schema to explicitly link to some external documentation such
      /// as `std` to use `std://` to link to the standard library, e.g., on
      /// cppreference.com.
      std::string schema;

      /// Whether no normal name lookups should be performed, e.g., for the
      /// standard library whether only `std://std::string` should resolve but
      /// not just `std::string`.
      // TODO(0.6.0-rc): Expose this option in the CLI.
      bool require_schema = false;
    };

    /// Create a transformer that will rewrite each
    /// [model::markup::link::target]() for all links in the [documents]() if
    /// they refer to any of the external [symbols]().
    set_target_external_transformer(model::entity_set* documents, inventory::symbols symbols, set_target_external_transformer_options options={});

  protected:
    void do_transform(model::entity&) override;

  private:
    const inventory::symbols symbols;

    const set_target_external_transformer_options options;
};

}

#endif


