// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_MODEL_VISITOR_DETAIL_VISIT_HPP_INCLUDED
#define STANDARDESE_MODEL_VISITOR_DETAIL_VISIT_HPP_INCLUDED

#include <type_safe/optional.hpp>
#include <type_traits>
#include <functional>

#include <boost/type_index.hpp>

#include "../visitor.hpp"
#include "../../entity.hpp"

// TODO: Have a look at the overloaded construction here:
// https://en.cppreference.com/w/cpp/utility/variant/visit
// maybe that can be used to compress this.
// Or maybe I can funnel everything through a single std::variant of pointers?

namespace standardese::model::visitor::detail
{

template <typename Callback>
using lambda_t = decltype(&Callback::operator());

template <typename T>
struct signature {
  static_assert(!std::is_same_v<T, T>, "Parameter is not a lambda. Cannot determine signature.");
};

template <typename T, typename _, typename ...Args>
struct signature<T(_::*)(Args...) const> {
  using return_type = T;

  template <size_t i>
  using arg_type = typename std::tuple_element<i, std::tuple<Args...>>::type;

  static constexpr size_t args = sizeof...(Args);
};

template <typename Callback>
using return_t = typename signature<lambda_t<Callback>>::return_type;

template <typename Callback, size_t i>
using arg_t = typename signature<lambda_t<Callback>>::template arg_type<i>;

template <typename Callback>
static constexpr size_t args_v = signature<lambda_t<Callback>>::args;

template <typename ...Callbacks>
struct visit_return {
  static_assert(sizeof...(Callbacks) > 0, "Cannot determine return type of visitor without any callbacks.");

  using type = typename std::common_type<return_t<Callbacks>...>::type;
};

template <typename ...Callbacks>
using visit_return_t = typename visit_return<Callbacks...>::type;

template <bool is_const, typename E>
using add_const_t = std::conditional_t<is_const, std::add_const_t<E>, E>;

template <bool is_const, typename T, typename E>
struct partial_visitor {
  using Parameter = add_const_t<is_const, E>;

  static_assert(std::is_same_v<E, std::remove_reference_t<std::remove_cv_t<E>>>, "Did not expect parameter E to have any qualifiers.");
  static_assert(std::is_same_v<void, decltype(std::declval<model::visitor::visitor<is_const>>().visit(std::declval<Parameter&>()))>, "Lambda cannot be used as a visitor.");

  using F = std::function<T(Parameter&)>;

  template <typename Callable>
  partial_visitor(Callable f) : implementation(std::move(f)) {}

  F implementation;
};

template <bool is_const, typename T>
struct default_visitor {
  using F = std::function<T()>;

  template <typename Callable>
  default_visitor(Callable f) : implementation(std::move(f)) {}

  F implementation;
};

template <bool is_const, typename ...Callbacks>
struct partial_visitors {
  using T = visit_return_t<Callbacks...>;

  template <typename Callback>
  struct visitor {
    static_assert(!std::is_same_v<Callback, Callback>, "Cannot deduce visitor signature from provided lambda.");
  };

  template <typename _>
  struct visitor<T(_::*)() const> {
    using type = default_visitor<is_const, T>;
  };

  template <typename _, typename Arg>
  struct visitor<T(_::*)(Arg&) const> {
    using type = partial_visitor<is_const, T, std::remove_cv_t<Arg>>;
  };

  template <typename Callback>
  using visitor_t = typename visitor<lambda_t<Callback>>::type;
};

template <typename T>
struct storage {
  type_safe::optional<T> value;
};

template <>
struct storage<void>{};

template<bool is_const, typename T, typename ...Bases>
class functional_visitor : public visitor<is_const>, storage<T>, Bases... {
  public:
    functional_visitor(typename Bases::F... callbacks) : Bases(std::move(callbacks))... {}

    virtual void visit(typename visitor<is_const>::block_quote& entity) override { eval(entity); }
    virtual void visit(typename visitor<is_const>::code& entity)  override { eval(entity); }
    virtual void visit(typename visitor<is_const>::code_block& entity)  override { eval(entity); }
    virtual void visit(typename visitor<is_const>::emphasis& entity)  override { eval(entity); }
    virtual void visit(typename visitor<is_const>::cpp_entity_documentation& entity)  override { eval(entity); }
    virtual void visit(typename visitor<is_const>::hard_break& entity)  override { eval(entity); }
    virtual void visit(typename visitor<is_const>::heading& entity)  override { eval(entity); }
    virtual void visit(typename visitor<is_const>::link& entity)  override { eval(entity); }
    virtual void visit(typename visitor<is_const>::list& entity)  override { eval(entity); }
    virtual void visit(typename visitor<is_const>::list_item& entity)  override { eval(entity); }
    virtual void visit(typename visitor<is_const>::module& entity)  override { eval(entity); }
    virtual void visit(typename visitor<is_const>::paragraph& entity)  override { eval(entity); }
    virtual void visit(typename visitor<is_const>::section& entity)  override { eval(entity); }
    virtual void visit(typename visitor<is_const>::soft_break& entity)  override { eval(entity); }
    virtual void visit(typename visitor<is_const>::strong_emphasis& entity)  override { eval(entity); }
    virtual void visit(typename visitor<is_const>::text& entity)  override { eval(entity); }
    virtual void visit(typename visitor<is_const>::thematic_break& entity)  override { eval(entity); }
    virtual void visit(typename visitor<is_const>::document& entity)  override { eval(entity); }
    virtual void visit(typename visitor<is_const>::image& entity)  override { eval(entity); }

    T operator ()(add_const_t<is_const, model::entity>& entity) {
      entity.accept(*this);
      if constexpr (!std::is_same_v<T, void>)
        return this->value.value();
    }

  private:
    template <typename F>
    void eval(F&& e) {
      using E = std::decay_t<F>;
      std::function<T()> callback;
      if constexpr ((std::is_same_v<partial_visitor<is_const, T, E>, Bases> || ...)) {
        callback = [&]() { return static_cast<partial_visitor<is_const, T, E>&>(*this).implementation(e); };
      } else if constexpr ((std::is_same_v<default_visitor<is_const, T>, Bases> || ...)) {
        callback = [&]() { return static_cast<default_visitor<is_const, T>&>(*this).implementation(); };
      } else {
        // TODO: Maybe use fmt instead in an implementation file.
        throw std::logic_error("not implemented: unexpected entity type `" + boost::typeindex::type_id<E*>().pretty_name() + "`; we did only expect one of the following " + ((boost::typeindex::type_id<Bases>().pretty_name() + ...)));
      }

      if constexpr (std::is_same_v<T, void>)
        callback();
      else
        this->value = callback();
    }
};

}

#endif
