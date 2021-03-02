// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_MODEL_MIXIN_VISITABLE_HPP_INCLUDED
#define STANDARDESE_MODEL_MIXIN_VISITABLE_HPP_INCLUDED

#include "../visitor/visitor.hpp"

namespace standardese::model::mixin
{

class ivisitable {
public:
  virtual void accept(visitor::visitor<false>& visitor) = 0;
  virtual void accept(visitor::visitor<true>& visitor) const = 0;

  virtual ivisitable* clone() const = 0;

  virtual ~ivisitable() {};
};

template <typename E>
class visitable : public ivisitable
{
public:
    void accept(visitor::visitor<false>& visitor) override {
        visitor.visit(*static_cast<E*>(this));
    }

    void accept(visitor::visitor<true>& visitor) const override {
        visitor.visit(*static_cast<const E*>(this));
    }

    ivisitable* clone() const override {
        return new E(*static_cast<const E*>(this));
    }
};

}

#endif

