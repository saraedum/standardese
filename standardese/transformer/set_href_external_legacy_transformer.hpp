// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_TRANSFORMER_SET_HREF_EXTERNAL_LEGACY_TRANSFORMER_HPP_INCLUDED
#define STANDARDESE_TRANSFORMER_SET_HREF_EXTERNAL_LEGACY_TRANSFORMER_HPP_INCLUDED

#include <string>

#include "inner_transformer.hpp"

namespace standardese::transformer
{

/// Resolve links to other documentations that use the deprecated
/// `--comment.external_doc` flag to link to a fixed search page URL.
class set_href_external_legacy_transformer : public inner_transformer {
  public:
    struct set_href_external_legacy_transformer_options {
      /// The base URL of the external documentation.
      std::string url;

      /// The C++ namespace to link to `url`.
      std::string namspace;
    };

    /// Create a transformer that sets the [model::markup::link::target]()
    /// attribute for each link in each of the [documents]() if it refers to an
    /// external entity.
    set_href_external_legacy_transformer(model::entity_set* documents, set_href_external_legacy_transformer_options);

  protected:
    void do_transform(model::entity&) override;

  private:
    const set_href_external_legacy_transformer_options options;
};

}

#endif
