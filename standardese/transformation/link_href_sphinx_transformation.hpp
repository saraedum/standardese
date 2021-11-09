// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_TRANSFORMATION_LINK_HREF_SPHINX_TRANSFORMATION_HPP_INCLUDED
#define STANDARDESE_TRANSFORMATION_LINK_HREF_SPHINX_TRANSFORMATION_HPP_INCLUDED

#include "transformation.hpp"

namespace standardese::transformation
{

class link_href_sphinx_transformation : public transformation {
  public:
    link_href_sphinx_transformation(model::unordered_entities& documents, const std::string& project, const std::string& version, const std::string& base_url);

  protected:
    void do_transform(model::entity&) override;

  private:
    const std::string project;
    const std::string version;
    const std::string base_url;
};

}

#endif



