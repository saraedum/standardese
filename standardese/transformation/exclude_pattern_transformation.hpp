// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_TRANSFORMATION_EXCLUDE_PATTERN_TRANSFORMATION_HPP_INCLUDED
#define STANDARDESE_TRANSFORMATION_EXCLUDE_PATTERN_TRANSFORMATION_HPP_INCLUDED

#include <regex>

#include "transformation.hpp"

namespace standardese::transformation {

class exclude_pattern_transformation : public transformation {
  public:
    struct options {
      std::vector<std::regex> excluded;
    };

  protected:
    void do_transform(model::entity& root) override;
};

}

#endif

