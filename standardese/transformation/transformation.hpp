// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_TRANSFORMATION_TRANSFORMATION_HPP_INCLUDED
#define STANDARDESE_TRANSFORMATION_TRANSFORMATION_HPP_INCLUDED

#include "../threading/unthreaded_pool.hpp"

namespace standardese::transformation {

class transformation {
  public:
    explicit transformation(model::unordered_entities& entities);

    virtual void transform(threading::pool::factory workers=threading::unthreaded_pool::factory);

  protected:
    virtual void do_transform(model::entity& root) = 0;

    model::unordered_entities& entities;
};

}

#endif
