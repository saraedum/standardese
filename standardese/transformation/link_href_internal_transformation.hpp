// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_TRANSFORMATION_LINK_HREF_INTERNAL_TRANSFORMATION_HPP_INCLUDED
#define STANDARDESE_TRANSFORMATION_LINK_HREF_INTERNAL_TRANSFORMATION_HPP_INCLUDED

#include <unordered_map>

#include "transformation.hpp"
#include "../model/unordered_entities.hpp"

namespace standardese::transformation
{

/// Rewrites MarkDown links that target a C++ entity as a link to an actual URL.
class link_href_internal_transformation : public transformation {
  public:
    link_href_internal_transformation(model::unordered_entities& documents);

  protected:
    void do_transform(model::entity&) override;

  private:
    std::unordered_map<const cppast::cpp_entity*, std::string> anchors;
};

}

#endif
