// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_MODEL_ENTITIES_HPP_INCLUDED
#define STANDARDESE_MODEL_ENTITIES_HPP_INCLUDED

#include <cppast/forward.hpp>
#include <memory>

#include "../forward.hpp"

namespace standardese::model
{

class unordered_entities {
    template <bool is_const>
    class unordered_iterator;

  public:
    using const_iterator = unordered_iterator<true>;
    using iterator = unordered_iterator<false>;
    using value_type = entity;

    unordered_entities() noexcept;
    ~unordered_entities() noexcept;

    unordered_entities(unordered_entities&&) noexcept;

    unordered_entities& operator=(unordered_entities&&) noexcept;

    template <typename R>
    explicit unordered_entities(const R& args) : unordered_entities(args.begin(), args.end()) {}

    template <typename It>
    unordered_entities(It begin, It end) : unordered_entities() {
      for(;begin != end; ++begin)
        insert(*begin);
    }

    void insert(value_type value);

    const_iterator find_cpp_entity(const cppast::cpp_entity& entity) const;
    iterator find_cpp_entity(const cppast::cpp_entity& entity);

    const_iterator find_module(const std::string&) const;
    iterator find_module(const std::string&);

    const model::entity& cpp_entity(const cppast::cpp_entity& entity) const;
    model::entity& cpp_entity(const cppast::cpp_entity& entity);

    const model::module& module(const std::string&) const;
    model::module& module(const std::string&);

    const_iterator begin() const;
    const_iterator end() const;

    iterator begin();
    iterator end();

  private:
    template <bool is_const>
    class unordered_iterator {
     public:
      unordered_iterator(const unordered_iterator&) noexcept;
      unordered_iterator(unordered_iterator&&) noexcept;
      ~unordered_iterator() noexcept;

      unordered_iterator& operator=(const unordered_iterator&) noexcept;
      unordered_iterator& operator=(unordered_iterator&&) noexcept;

      bool operator==(const unordered_iterator&) const;
      bool operator!=(const unordered_iterator&) const;

      unordered_iterator& operator++();
      std::conditional_t<is_const, const entity&, entity&> operator*() const;
      std::conditional_t<is_const, const entity*, entity*> operator->() const;

     private:
      unordered_iterator() noexcept;
      friend class unordered_entities;

      struct impl;

      std::unique_ptr<impl> impl_;
    };

    struct impl;

    std::unique_ptr<impl> impl_;
};

}

#endif
