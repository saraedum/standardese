// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <boost/unordered_set.hpp>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <initializer_list>

#include "../../standardese/model/unordered_entities.hpp"
#include "../../standardese/model/entity.hpp"
#include "../../standardese/model/cpp_entity_documentation.hpp"
#include "../../standardese/model/module.hpp"
#include "../../standardese/model/visitor/visit.hpp"
#include "../../standardese/logger.hpp"
#include "../../standardese/output_generator/xml/xml_generator.hpp"

namespace standardese::model {

struct unordered_entities::unordered_entities_implementation {
  struct hash {
    size_t operator()(const entity&) const;
  };

  struct equality {
    bool operator()(const entity& lhs, const entity& rhs) const;
  };

  boost::unordered_set<entity, hash, equality> items;
};

template <bool is_const>
struct unordered_entities::unordered_iterator<is_const>::unordered_iterator_implementation {
  boost::unordered_set<entity>::iterator self;
};

unordered_entities::unordered_entities() noexcept : self(new unordered_entities_implementation{}) {}

unordered_entities::~unordered_entities() noexcept {}

unordered_entities::unordered_entities(std::initializer_list<model::entity> init) : unordered_entities(init.begin(), init.end()) {}

unordered_entities::unordered_entities(unordered_entities&& rhs) noexcept : self(std::move(rhs.self)) {}

unordered_entities& unordered_entities::operator=(unordered_entities&& rhs) noexcept {
  self = std::move(rhs.self);
  return *this;
}

void unordered_entities::insert(value_type value) {
  auto [pos, inserted] = self->items.insert(value);
  if (!inserted)
    logger::warn(fmt::format("Not adding entity {} because an equivalent entity was already found in this set.", value));
}

unordered_entities::const_iterator unordered_entities::find_cpp_entity(const cppast::cpp_entity& entity) const {
  struct hash {
    size_t operator()(const cppast::cpp_entity& entity) const {
      return reinterpret_cast<size_t>(&entity);
    }
  };

  struct equality {
    bool operator()(const cppast::cpp_entity& lhs, const struct entity& rhs) const {
      // TODO(0.6.0-rc): Search group_documentation?
      return rhs.is<cpp_entity_documentation>() && &rhs.as<cpp_entity_documentation>().entity() == &lhs;
    }
  };

  auto it =  self->items.find(entity, hash{}, equality{});

  const_iterator ret;
  ret.self->self = it;

  return ret;
}

unordered_entities::iterator unordered_entities::find_cpp_entity(const cppast::cpp_entity& entity) {
  auto it = const_cast<const unordered_entities*>(this)->find_cpp_entity(entity).self->self;

  iterator ret;
  ret.self->self = it;

  return ret;
}

unordered_entities::const_iterator unordered_entities::find_module(const std::string& name) const {
  auto it = self->items.find(model::module(name));

  const_iterator ret;
  ret.self->self = it;

  return ret;
}

unordered_entities::iterator unordered_entities::find_module(const std::string& name) {
  auto it = const_cast<const unordered_entities*>(this)->find_module(name).self->self;

  iterator ret;
  ret.self->self = it;

  return ret;
}

const model::entity& unordered_entities::cpp_entity(const cppast::cpp_entity& entity) const {
  auto it = find_cpp_entity(entity);

  if (it == end())
    throw std::invalid_argument(fmt::format("entity `{}` not found in entities", entity.name()));

  return *it;
}

void unordered_entities::erase(const_iterator pos) {
  self->items.erase(pos.self->self);
}

void unordered_entities::erase(iterator pos) {
  self->items.erase(pos.self->self);
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
  ret.self->self = self->items.begin();
  return ret;
}

unordered_entities::const_iterator unordered_entities::end() const {
  const_iterator ret;
  ret.self->self = self->items.end();
  return ret;
}

unordered_entities::iterator unordered_entities::begin() {
  iterator ret;
  ret.self->self = self->items.begin();
  return ret;
}

unordered_entities::iterator unordered_entities::end() {
  iterator ret;
  ret.self->self = self->items.end();
  return ret;
}

size_t unordered_entities::size() const {
  return self->items.size();
}

template <bool is_const>
unordered_entities::unordered_iterator<is_const>::unordered_iterator() noexcept : self(new unordered_iterator_implementation{}) {};

template <bool is_const>
unordered_entities::unordered_iterator<is_const>::unordered_iterator(const unordered_iterator& value) noexcept : self(new unordered_iterator_implementation{value.self->self}) {}

template <bool is_const>
unordered_entities::unordered_iterator<is_const>::unordered_iterator(unordered_iterator&& value) noexcept : self(std::move(value.self)) {}

template <bool is_const>
unordered_entities::unordered_iterator<is_const>::~unordered_iterator() noexcept {}

template <bool is_const>
unordered_entities::unordered_iterator<is_const>& unordered_entities::unordered_iterator<is_const>::operator=(const unordered_iterator& rhs) noexcept {
  self->self = rhs.self->self;
  return *this;
}

template <bool is_const>
unordered_entities::unordered_iterator<is_const>& unordered_entities::unordered_iterator<is_const>::operator=(unordered_iterator&& rhs) noexcept {
  self = std::move(rhs.self);
  return *this;
}

template <bool is_const>
bool unordered_entities::unordered_iterator<is_const>::operator==(const unordered_iterator& rhs) const {
  return self->self == rhs.self->self;
}

template <bool is_const>
bool unordered_entities::unordered_iterator<is_const>::operator!=(const unordered_iterator& rhs) const {
  return !(*this == rhs);
}

template <bool is_const>
unordered_entities::unordered_iterator<is_const>& unordered_entities::unordered_iterator<is_const>::operator++() {
  self->self++;
  return *this;
}

template <bool is_const>
std::conditional_t<is_const, const entity&, entity&> unordered_entities::unordered_iterator<is_const>::operator*() const {
  if constexpr (is_const) {
    return *self->self;
  } else {
    return const_cast<entity&>(*self->self);
  }
}

template <bool is_const>
std::conditional_t<is_const, const entity*, entity*> unordered_entities::unordered_iterator<is_const>::operator->() const {
  if constexpr (is_const) {
    return &*self->self;
  } else {
    return const_cast<entity*>(&*self->self);
  }
}

size_t unordered_entities::unordered_entities_implementation::hash::operator()(const entity& self) const {
  return visitor::visit([&](auto&& entity) {
    using T = std::decay_t<decltype(entity)>;
    // TODO(0.6.0-rc): Handle group_documentation?
    if constexpr (std::is_same_v<T, model::cpp_entity_documentation>) {
      return reinterpret_cast<size_t>(&entity.entity());
    } else if constexpr (std::is_same_v<T, model::module>) {
      return std::hash<std::string>()(entity.name);
    } else {
      return reinterpret_cast<size_t>(self.get());
    }
  }, self);
}

bool unordered_entities::unordered_entities_implementation::equality::operator()(const entity& lhs, const entity& rhs) const {
  return visitor::visit([&](auto&& lentity) {
      using T = std::decay_t<decltype(lentity)>;
      // TODO(0.6.0-rc): Handle group_documentation?
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
