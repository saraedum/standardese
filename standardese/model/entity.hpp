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
    // TODO(0.6.0-beta): This implicit cast is maybe not such a great idea. It can be quite confusing when it happens. And it implies a copy being created.
    template <typename E, std::enable_if_t<std::is_base_of_v<mixin::ivisitable, std::decay_t<E>>, bool> Enabled = true>
    entity(E&& e) : value(new std::decay_t<E>(std::forward<E>(e))) {}

    entity(const entity& rhs);
    entity(entity&& rhs);

    entity& operator=(const entity& rhs);

    entity& operator=(entity&& rhs);

    void accept(visitor::visitor<false>& visitor);

    void accept(visitor::visitor<true>& visitor) const;

    const mixin::ivisitable* get() const;

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

    friend std::ostream& operator<<(std::ostream&, const entity&);

  private:
    std::unique_ptr<mixin::ivisitable> value;
};

}

#endif // STANDARDESE_MODEL_ENTITY_HPP_INCLUDED
