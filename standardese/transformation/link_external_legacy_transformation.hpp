// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_TRANSFORMATION_LINK_EXTERNAL_LEGACY_TRANSFORMATION_HPP_INCLUDED
#define STANDARDESE_TRANSFORMATION_LINK_EXTERNAL_LEGACY_TRANSFORMATION_HPP_INCLUDED

#include "transformation.hpp"

namespace standardese::transformation
{

/// Resolve links to other documentations that use the deprecated
/// `--comment.external_doc` flag to link to a fixed search page URL.
class link_external_legacy_transformation : public transformation {
  public:
    struct options {
      /// The base URL of the external documentation.
      std::string url;

      /// The C++ namespace to link to `url`.
      std::string namspace;
    };

    link_external_legacy_transformation(model::unordered_entities& documents, options);

  protected:
    void do_transform(model::entity&) override;

  private:
    const options options;
};

}

#endif
