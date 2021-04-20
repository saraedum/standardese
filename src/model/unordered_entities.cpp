// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <boost/unordered_set.hpp>
#include <fmt/format.h>

#include "../../standardese/model/unordered_entities.hpp"
#include "../../standardese/model/entity.hpp"
#include "../../standardese/model/cpp_entity_documentation.hpp"
#include "../../standardese/model/module.hpp"
#include "../../standardese/model/visitor/visit.hpp"

namespace standardese::model {

struct unordered_entities::impl {
  struct hash {
    size_t operator()(const entity&) const;
  };

  struct equality {
    bool operator()(const entity& lhs, const entity& rhs) const;
  };

  boost::unordered_set<entity, hash, equality> items;
};

template <bool is_const>
struct unordered_entities::unordered_iterator<is_const>::impl {
  boost::unordered_set<entity>::iterator self;
};

unordered_entities::unordered_entities() noexcept : impl_(new impl{}) {}

unordered_entities::~unordered_entities() noexcept {}

unordered_entities::unordered_entities(unordered_entities&& rhs) noexcept : impl_(std::move(rhs.impl_)) {}

unordered_entities& unordered_entities::operator=(unordered_entities&& rhs) noexcept {
  impl_ = std::move(rhs.impl_);
  return *this;
}

void unordered_entities::insert(value_type value) {
  impl_->items.insert(value);
}

unordered_entities::const_iterator unordered_entities::find_cpp_entity(const cppast::cpp_entity& entity) const {
  struct hash {
    size_t operator()(const cppast::cpp_entity& entity) const {
      return reinterpret_cast<size_t>(&entity);
    }
  };

  struct equality {
    bool operator()(const cppast::cpp_entity& lhs, const struct entity& rhs) const {
      return rhs.is<cpp_entity_documentation>() && &rhs.as<cpp_entity_documentation>().entity() == &lhs;
    }
  };

  auto it =  impl_->items.find(entity, hash{}, equality{});

  const_iterator ret;
  ret.impl_->self = it;

  return ret;
}

unordered_entities::iterator unordered_entities::find_cpp_entity(const cppast::cpp_entity& entity) {
  auto it = const_cast<const unordered_entities*>(this)->find_cpp_entity(entity).impl_->self;

  iterator ret;
  ret.impl_->self = it;

  return ret;
}

unordered_entities::const_iterator unordered_entities::find_module(const std::string& name) const {
  auto it = impl_->items.find(model::module(name));

  const_iterator ret;
  ret.impl_->self = it;

  return ret;
}

unordered_entities::iterator unordered_entities::find_module(const std::string& name) {
  auto it = const_cast<const unordered_entities*>(this)->find_module(name).impl_->self;

  iterator ret;
  ret.impl_->self = it;

  return ret;
}

const model::entity& unordered_entities::cpp_entity(const cppast::cpp_entity& entity) const {
  auto it = find_cpp_entity(entity);

  if (it == end())
    throw std::invalid_argument(fmt::format("entity `{}` not found in entities", entity.name()));

  return *it;
}

model::entity& unordered_entities::cpp_entity(const cppast::cpp_entity& entity) {
  return const_cast<model::entity&>(const_cast<const unordered_entities*>(this)->cpp_entity(entity));
}

const model::module& unordered_entities::module(const std::string& name) const {
  auto it = find_module(name);

  if (it == end())
    throw std::invalid_argument(fmt::format("module `{}` not found in entities", name));

  return it->as<model::module>();
}

model::module& unordered_entities::module(const std::string& name) {
  return const_cast<model::module&>(const_cast<const unordered_entities*>(this)->module(name));
}

unordered_entities::const_iterator unordered_entities::begin() const {
  const_iterator ret;
  ret.impl_->self = impl_->items.begin();
  return ret;
}

unordered_entities::const_iterator unordered_entities::end() const {
  const_iterator ret;
  ret.impl_->self = impl_->items.end();
  return ret;
}

unordered_entities::iterator unordered_entities::begin() {
  iterator ret;
  ret.impl_->self = impl_->items.begin();
  return ret;
}

unordered_entities::iterator unordered_entities::end() {
  iterator ret;
  ret.impl_->self = impl_->items.end();
  return ret;
}

template <bool is_const>
unordered_entities::unordered_iterator<is_const>::unordered_iterator() noexcept : impl_(new impl{}) {};

template <bool is_const>
unordered_entities::unordered_iterator<is_const>::unordered_iterator(const unordered_iterator& value) noexcept : impl_(new impl{value.impl_->self}) {}

template <bool is_const>
unordered_entities::unordered_iterator<is_const>::unordered_iterator(unordered_iterator&& value) noexcept : impl_(std::move(value.impl_)) {}

template <bool is_const>
unordered_entities::unordered_iterator<is_const>::~unordered_iterator() noexcept {}

template <bool is_const>
unordered_entities::unordered_iterator<is_const>& unordered_entities::unordered_iterator<is_const>::operator=(const unordered_iterator& rhs) noexcept {
  impl_->self = rhs.impl_->self;
  return *this;
}

template <bool is_const>
unordered_entities::unordered_iterator<is_const>& unordered_entities::unordered_iterator<is_const>::operator=(unordered_iterator&& rhs) noexcept {
  impl_ = std::move(rhs.impl_);
  return *this;
}

template <bool is_const>
bool unordered_entities::unordered_iterator<is_const>::operator==(const unordered_iterator& rhs) const {
  return impl_->self == rhs.impl_->self;
}

template <bool is_const>
bool unordered_entities::unordered_iterator<is_const>::operator!=(const unordered_iterator& rhs) const {
  return !(*this == rhs);
}

template <bool is_const>
unordered_entities::unordered_iterator<is_const>& unordered_entities::unordered_iterator<is_const>::operator++() {
  impl_->self++;
  return *this;
}

template <bool is_const>
std::conditional_t<is_const, const entity&, entity&> unordered_entities::unordered_iterator<is_const>::operator*() const {
  if constexpr (is_const) {
    return *impl_->self;
  } else {
    return const_cast<entity&>(*impl_->self);
  }
}

template <bool is_const>
std::conditional_t<is_const, const entity*, entity*> unordered_entities::unordered_iterator<is_const>::operator->() const {
  if constexpr (is_const) {
    return &*impl_->self;
  } else {
    return const_cast<entity*>(&*impl_->self);
  }
}

size_t unordered_entities::impl::hash::operator()(const entity& self) const {
  return visitor::visit([&](auto&& entity) {
    using T = std::decay_t<decltype(entity)>;
    if constexpr (std::is_same_v<T, model::cpp_entity_documentation>) {
      return reinterpret_cast<size_t>(&entity.entity());
    } else if constexpr (std::is_same_v<T, model::module>) {
      return std::hash<std::string>()(entity.name);
    } else {
      return reinterpret_cast<size_t>(self.get());
    }
  }, self);
}

bool unordered_entities::impl::equality::operator()(const entity& lhs, const entity& rhs) const {
  return visitor::visit([&](auto&& lentity) {
      using T = std::decay_t<decltype(lentity)>;
      if constexpr (std::is_same_v<T, model::cpp_entity_documentation>) {
        return visitor::visit([&](auto&& rentity) {
          using S = std::decay_t<decltype(rentity)>;
          if constexpr (std::is_same_v<S, T>) {
            return &lentity.entity() == &rentity.entity();
          }
          return false;
        }, rhs);
      } else if constexpr (std::is_same_v<T, model::module>) {
        return visitor::visit([&](auto&& rentity) {
          using S = std::decay_t<decltype(rentity)>;
          if constexpr (std::is_same_v<S, T>) {
            return lentity.name == rentity.name;
          }
          return false;
        }, rhs);
      }
      return lhs.get() == rhs.get();
  }, lhs);
}

template class unordered_entities::unordered_iterator<true>;
template class unordered_entities::unordered_iterator<false>;

}
