// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_TRANSFORMATION_LINK_SPHINX_TRANSFORMATION_HPP_INCLUDED
#define STANDARDESE_TRANSFORMATION_LINK_SPHINX_TRANSFORMATION_HPP_INCLUDED

#include "link_target_external_transformation.hpp"
#include "../inventory/sphinx/documentation_set.hpp"
#include "../inventory/symbols.hpp"

namespace standardese::transformation
{

/// Resolves links to project documentations generated with Sphinx.
class link_sphinx_transformation : public transformation {
  public:
    struct options : link_target_external_transformation::options {
      /// The base URL of the external documentation.
      std::string url;
    };

    link_sphinx_transformation(model::unordered_entities& documents, options options, inventory::sphinx::documentation_set inventory);

    void transform(threading::pool::factory workers=threading::unthreaded_pool::factory) override;

  protected:
    void do_transform(model::entity&) override;

  private:
    const options options;
    const inventory::sphinx::documentation_set inventory;
    link_target_external_transformation target_transformation;
};

}

#endif



