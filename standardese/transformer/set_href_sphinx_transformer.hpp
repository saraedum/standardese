// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_TRANSFORMER_SET_HREF_SPHINX_TRANSFORMER_HPP_INCLUDED
#define STANDARDESE_TRANSFORMER_SET_HREF_SPHINX_TRANSFORMER_HPP_INCLUDED

#include "set_target_external_transformer.hpp"
#include "../inventory/sphinx/documentation_set.hpp"
#include "../inventory/symbols.hpp"

namespace standardese::transformer
{

/// Resolves links to project documentations generated with Sphinx.
class set_href_sphinx_transformer : public transformer {
  public:
    struct options : set_target_external_transformer::options {
      /// The base URL of the external documentation.
      std::string url;
    };

    set_href_sphinx_transformer(model::unordered_entities& documents, options options, inventory::sphinx::documentation_set inventory);

    void transform(threading::pool::factory workers=threading::unthreaded_pool::factory) override;

  protected:
    void do_transform(model::entity&) override;

  private:
    const options options;
    const inventory::sphinx::documentation_set inventory;
    set_target_external_transformer target_transformer;
};

}

#endif



