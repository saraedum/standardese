// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_TRANSFORMER_SET_HREF_EXTERNAL_LEGACY_TRANSFORMER_HPP_INCLUDED
#define STANDARDESE_TRANSFORMER_SET_HREF_EXTERNAL_LEGACY_TRANSFORMER_HPP_INCLUDED

#include "transformer.hpp"

namespace standardese::transformer
{

/// Resolve links to other documentations that use the deprecated
/// `--comment.external_doc` flag to link to a fixed search page URL.
class set_href_external_legacy_transformer : public transformer {
  public:
    struct options {
      /// The base URL of the external documentation.
      std::string url;

      /// The C++ namespace to link to `url`.
      std::string namspace;
    };

    set_href_external_legacy_transformer(model::unordered_entities& documents, options);

  protected:
    void do_transform(model::entity&) override;

  private:
    const options options;
};

}

#endif
