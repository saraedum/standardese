// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_TRANSFORMER_LINK_TARGET_EXTERNAL_TRANSFORMER_HPP_INCLUDED
#define STANDARDESE_TRANSFORMER_LINK_TARGET_EXTERNAL_TRANSFORMER_HPP_INCLUDED

#include "transformer.hpp"
#include "../inventory/symbols.hpp"

// TODO(0.6.0-alpha): Make sure that we have consistent calling conventions in constructors/factories everywhere but also make sure it's always documented when the caller is expected to keep something alive:
// * const&: the caller has to keep the value alive (with exception of std::string)

namespace standardese::transformer
{

/// Resolves links in MarkDown that use standardese syntax to link to an
/// external target such another standardese project, or documentation
/// generated with doxygen or sphinx.
class link_target_external_transformer : public transformer {
  public:
    struct options {
      /// A URI schema to explicitly link to some external documentation such
      /// as `std` to use `std://` to link to the standard library.
      /// TODO(0.6.0-alpha): Implement this.
      std::string schema;

      /// Whether no normal name lookups should be performed, e.g., for the
      /// standard library whether only `std://std::string` should resolve but
      /// not just `std::string`.
      /// TODO(0.6.0-alpha): Implement this.
      bool require_schema = false;
    };

    link_target_external_transformer(model::unordered_entities& documents, inventory::symbols symbols);

  protected:
    void do_transform(model::entity&) override;

  private:
    const inventory::symbols symbols;
};

}

#endif


