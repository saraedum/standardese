// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_TRANSFORMATION_GROUP_TRANSFORMATION_HPP_INCLUDED
#define STANDARDESE_TRANSFORMATION_GROUP_TRANSFORMATION_HPP_INCLUDED

#include "transformation.hpp"

namespace standardese::transformation {

class group_transformation : public transformation {
  public:
    struct group_options {
    };

    explicit group_transformation(model::unordered_entities& documents, struct group_options options);

  protected:
    void do_transform(model::entity& root) override;

  private:
    void merge(model::group_documentation&, model::cpp_entity_documentation&&) const;

    group_options options;
};

}

#endif

