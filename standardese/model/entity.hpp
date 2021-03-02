// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_MODEL_ERASED_ENTITY_HPP_INCLUDED
#define STANDARDESE_MODEL_ERASED_ENTITY_HPP_INCLUDED

#include <stdexcept>
#include <memory>
#include <type_traits>

#include "visitor/visitor.hpp"
#include "mixin/visitable.hpp"

namespace standardese::model
{

class entity {
  public:
    template <typename E, std::enable_if_t<std::is_base_of_v<mixin::ivisitable, std::decay_t<E>>, bool> Enabled = true>
    entity(E&& e) : value(new std::decay_t<E>(std::forward<E>(e))) {}

    entity(const entity& rhs) : value(rhs.value->clone()) {}
    entity(entity&& rhs) : value(std::move(rhs.value)) {}

    entity& operator=(entity& rhs) {
      value.reset(rhs.value->clone());
      return *this;
    }

    entity& operator=(entity&& rhs) {
      value = std::move(rhs.value);
      return *this;
    }

    void accept(visitor::visitor<false>& visitor) {
        value->accept(visitor);
    }

    void accept(visitor::visitor<true>& visitor) const {
        value->accept(visitor);
    }

    const mixin::ivisitable* get() const {
        return value.get();
    }

    template <typename T>
    const T& as() const {
      const T* t = dynamic_cast<const T*>(value.get());
      if (t == nullptr)
        throw std::invalid_argument("this entity is not of the required type");

      return *t;
    }

    template <typename T>
    T& as() {
      return const_cast<T&>(const_cast<const entity&>(*this).as<T>());
    }

    template <typename T>
    bool is() const {
      return dynamic_cast<const T*>(value.get()) != nullptr;
    }

  private:
    std::unique_ptr<mixin::ivisitable> value;
};

}

#endif // STANDARDESE_MODEL_ENTITY_HPP_INCLUDED
